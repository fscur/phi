#ifndef _PHI_FLOAT_ANIMATION_H_
#define _PHI_FLOAT_ANIMATION_H_

namespace phi
{
    class floatAnimation
    {
    private:
        float* _value;
        float _from;
        float _to;
        int _milliseconds;
        int _elapsed;

    public:
        floatAnimation(float* value, float to, int milliseconds)
        {
            _value = value;
            _from = *value;
            _to = to;
            _milliseconds = milliseconds;
            _elapsed = 0;
        }

        float* getValue() { return _value; }
        float getFrom() { return _from; }
        float getTo() { return _to; }
        int getMilliseconds() { return _milliseconds; }
        int getElapsed() { return _elapsed; }

        void setElapsed(int value) { _elapsed = value; }
    };
}

#endif