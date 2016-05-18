#include <precompiled.h>
#include "obb.h"

namespace phi
{
    void obb::project(const vec3& axis, float& min, float& max) const
    {
        float rx = glm::abs(glm::dot(axis, axes[0])) * halfSizes[0];
        float ry = glm::abs(glm::dot(axis, axes[1])) * halfSizes[1];
        float rz = glm::abs(glm::dot(axis, axes[2])) * halfSizes[2];

        float c = glm::dot(center, axis);
        float r = rx + ry + rz;

        min = c - r;
        max = c + r;
    }

    bool obb::testAxisIntersection(const vec3& axis, float axis_length_squared, float d0, float d1, vec3& mtd, float& mtd2)
    {
        // intervals do not overlap. no intersection.
        if (d0 <= 0.0f || d1 <= 0.0f)
            return false;

        // find out if we overlap on the 'right' or 'left' of the object.
        float overlap = (d0 < d1) ? d0 : -d1;

        // the mtd vector for that axis
        vec3 sep = axis * (overlap / axis_length_squared);

        // the mtd vector length squared.
        float sep_length_squared = glm::dot(sep, sep);

        // if that vector is smaller than our computed MTD (or the mtd hasn't been computed yet)
        // use that vector as our current mtd.
        if (sep_length_squared < mtd2 || mtd2 < 0.0f)
        {
            mtd2 = sep_length_squared;
            mtd = sep;
        }
        return true;
    }

    bool obb::testAxisCollision(const vec3& axis, float axis_length_squared, float d0, float d1, float v,
        vec3& Nenter, vec3& Nleave, float& tenter, float& tleave)
    {
        // velocity perpendicular to axis or axis degenerate or velocity too small. ignore test.
        if (fabs(v) < 1.0E-8f)
        {
            // if intervals do not overlap, no collision.
            return (d0 >= 0.0f && d1 >= 0.0f);
        }

        vec3 N0 = axis;
        vec3 N1 = -axis;
        float t0 = d0 / v;  // estimated time of collision to the 'left' side
        float t1 = -d1 / v;  // estimated time of collision to the 'right' side

                             // sort values on axis so we have a valid swept interval.
        if (t0 > t1)
        {
            float ttemp = t0;
            t0 = t1;
            t1 = ttemp;

            vec3 Ntemp = N0;
            N0 = N1;
            N1 = Ntemp;
        }

        // swept interval outside our range.
        if (t1 < 0.0f) // || t0 > 1.0f)
            return false;

        // first swept test. 
        if (tenter > tleave)
        {
            tenter = t0;
            tleave = t1;
            Nenter = N0;
            Nleave = N1;
            return true;
        }
        // collision missed.
        if (t0 > tleave || t1 < tenter)
            return false;

        // minimise the collision interval
        if (t0 > tenter)
        {
            tenter = t0;
            Nenter = N0;
        }

        if (t1 < tleave)
        {
            tleave = t1;
            Nleave = N1;
        }

        // colliding.
        return true;
    }

    bool obb::testAxis(const vec3& axis, const obb& a, const obb& b, const vec3& vel,
        vec3& mtd, float& mtd2, bool& intersecting,
        vec3& Nenter, vec3& Nleave, float& tenter, float& tleave, bool& colliding)
    {
        // the axis length squared
        float axis_length_squared = glm::dot(axis, axis);
        if (axis_length_squared < 1.0E-8f)
            return true;

        // intervals along axis
        float mina, maxa;
        float minb, maxb;
        a.project(axis, mina, maxa);
        b.project(axis, minb, maxb);

        // calculate the two possible overlap ranges.
        // either we overlap on the left or right sides.
        float d0 = (maxb - mina); // 'left' side
        float d1 = (maxa - minb); // 'right' side
        float v = glm::dot(axis, vel);  // project velocity on axis for swept tests

                                 // test intersections
        intersecting &= testAxisIntersection(axis, axis_length_squared, d0, d1, mtd, mtd2);

        // test collisions
        colliding &= testAxisCollision(axis, axis_length_squared, d0, d1, v, Nenter, Nleave, tenter, tleave);

        // we should be either intersecting or colliding
        return (intersecting || colliding);
    }

    intersectionResult obb::interesects(const obb& a, const vec3& va, const obb& b, const vec3& vb)
    {
        intersectionResult result;

        // initialise to impossible values.
        auto mtd2 = -1.0f;
        auto t_enter = 1.0f;
        auto t_leave = 0.0f;

        // the normals of the faces when we first collide, and when the two objects stop intersecting along the velocity vector.
        vec3 n_enter;
        vec3 n_leave;

        // relative velocity of the boxes.
        vec3 vel = (va - vb);

        // test a face dirs
        for (int i = 0; i < 3; i++)
        {
            if (!testAxis(a.axes[i],
                a, b, vel,
                result.mtd, mtd2, result.intersecting,
                n_enter, n_leave, t_enter, t_leave, result.colliding))
            {
                result.colliding = result.intersecting = false;
                return result;
            }
        }

        // test b face dirs
        for (int i = 0; i < 3; i++)
        {
            if (!testAxis(b.axes[i],
                a, b, vel,
                result.mtd, mtd2, result.intersecting,
                n_enter, n_leave, t_enter, t_leave, result.colliding))
            {
                result.colliding = result.intersecting = false;
                return result;
            }
        }

        // test edges cross dirs
        for (int i = 0; i < 3; i++)
        {
            const vec3& a_edge = a.axes[i];

            for (int j = 0; j < 3; j++)
            {
                const vec3& b_edge = b.axes[j];

                vec3 axis = glm::cross(a_edge, b_edge);

                if (!testAxis(axis,
                    a, b, vel,
                    result.mtd, mtd2, result.intersecting,
                    n_enter, n_leave, t_enter, t_leave, result.colliding))
                {
                    return result;
                }
            }
        }

        // no intersections were ever tested.
        if (mtd2 < 0.0f)
            result.intersecting = false;

        // no collision were ever tested.
        if (t_enter > t_leave)
            result.colliding = false;

        // intersection failed. reset mtd to 0.
        if (!result.intersecting)
            result.mtd = vec3(0.0f);

        // we have collided. setup collision params.
        if (result.colliding)
        {
            result.time = t_enter;
            result.normal = glm::normalize(n_enter);
        }
        // no collision reported (box could have the same velocity).
        // in that case, use the mtd as normal of collision.
        else
        {
            if (result.intersecting)
                result.normal = glm::normalize(result.mtd);
        }

        // return intersection or collision.
        return result;
    }
}