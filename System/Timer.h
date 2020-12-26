#ifndef _RTETIMER_
#define _RTETIMER_

#include "RTEError.h"

namespace RTE {
	/// <summary>
	/// This is here just so everything compiles because GUI relies on RTE Timer but it's irrelevant in the editor.
	/// </summary>
	class Timer {
	public:
		Timer() = default;
		void Reset() { return; }
		double GetElapsedRealTimeMS() const { return 0; }
		double GetElapsedRealTimeS() const { return 0; }
	};
}
#endif