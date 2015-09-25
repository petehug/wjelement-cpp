#ifndef _HeapChecker_h_
#define _HeapChecker_h_

// In your code, wrap any HeapChecker code like this:
// #ifdef _HAS_HEAP_CHECKER
//   ...heapchecker code here
// #endif 

// Comment this line out if you don't want heap checking enabled 
#ifdef _DEBUG
#define _HAS_HEAP_CHECKER
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <string.h>

#endif


// Do some debug memory checking
//
class HeapChecker
{
#ifdef _DEBUG
	protected:
		_CrtMemState	cdbgMS1, cdbgMS2, cdbgMS3;

	public:
		static HeapChecker		M_HeapChecker;

		HeapChecker(bool bCreateIntentionalLeak = false)
		{
			_CrtSetDbgFlag( _CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(0));
			_CrtMemCheckpoint( &cdbgMS1 );

			DebugCheckpoint("At program start");

			if (bCreateIntentionalLeak)
				Leak();
		}

		~HeapChecker()
		{
			DebugCheckpoint("At program exit");
		}

		void Leak(char* msg="Leak Intentional!")
		{
			char* c;
			c = new char[100];
			strcpy(c,msg);
		}

		void	DebugCheckpoint(const char* pText)
		{
			char		szMessage[200];

			if (pText)
				strcpy(szMessage, pText);
			else
				strcpy(szMessage, "Debug - Memory usage at this stage is:");

			_RPT1( _CRT_WARN, "\n\n%s:\n", szMessage);
			memset(szMessage, '*', strlen(szMessage));
			_RPT1( _CRT_WARN, "%s\n", szMessage);
			_CrtMemCheckpoint( &cdbgMS2 );
			_CrtMemDifference( &cdbgMS3, &cdbgMS1, &cdbgMS2 );
			_CrtMemDumpStatistics( &cdbgMS3 );
		}


		long	BreakOnAllocation(long lAllocNum)
		{
			return _CrtSetBreakAlloc(lAllocNum);
		}

#else
	public:
		static	HeapChecker		M_HeapChecker;

		HeapChecker(bool bCreateIntentionalLeak = false) {}
		~HeapChecker() {}

		void		Leak(char* msg="Leak Intentional!") {}
		void		DebugCheckpoint(const char* pText) {}
		long		BreakOnAllocation(long lAllocNum) { return 0; }
#endif
};

#ifdef _DEBUG
	HeapChecker	HeapChecker::M_HeapChecker;
	
	#ifdef HEAPCHECKER_BREAK_ON
		long G_l = HeapChecker::M_HeapChecker.BreakOnAllocation(HEAPCHECKER_BREAK_ON);
	#endif
#endif

#endif /* _HeapChecker_h_ */
