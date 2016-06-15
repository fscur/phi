#pragma once
#include <phi.h>

#include "shaderCompilationResult.h"

namespace phi
{
    class programCompilationResult
    {
    private:
        bool _succeeded;
        vector<shaderCompilationResult> _shadersResult;

    public:
        programCompilationResult() :
            _succeeded(true)
        {
        }

        void add(shaderCompilationResult shaderResult)
        {
            _succeeded &= shaderResult.getSucceeded();
            _shadersResult.push_back(shaderResult);
        }

        string toString()
        {
            auto message = string();

            for (auto& result : _shadersResult)
                message.append(result.toString());

            return message;
        }

        bool getSucceeded()
        {
            return _succeeded;
        }
    };
}