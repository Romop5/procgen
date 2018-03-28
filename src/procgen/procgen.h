/*
 * API
 */
#ifndef PROCGEN_H
#define PROCGEN_H

#include <procgen/parser/generation.h>

namespace ProcGen {
	class Procgen {
        ProcGen::Generation generation;
		public:
		Procgen();
        ~Procgen();

		bool parseFile(const std::string& file);

        // Run inicialization
        bool runInit();

        // Run real derivation
        bool run(int maximumSteps);

        json serialize() const;

		bool isReady()	{ return generation.flagIsParsed; }

        template<typename T>
        bool setUniform(std::string uniformName,T value)
        {
            return generation.setUniform(uniformName, value);
        }
         
        void setDebugOn(bool state);

        // Clean
        void reinitialize();
    };
}
#endif
