#include <string.h>
#include "wildcards.h"

enum _wcstates
{
	WCS_CMP,
	WCS_SEARCH,
	WCS_SEARCH2,
	WCS_MATCH,
	WCS_MISMATCH
}

int wildcardfit(const char* pWildcard, const char* pString)
{
	enum _wcstates stt = WCS_CMP;
	/* Position im Wildcard-String */
	char* pWildcardLast;

	while (stt != WCS_MATCH && stt != WCS_MISMATCH)
	{
		switch (stt)
		{
			/* In diesem Zustand werden die zwei Strings solange verglichen bis... */
			case WCS_CMP:
				/* a) Ein * auftritt */
				if ('*' == *pWildcard)
				{
					/* Dann gehen wir hinter den * */
					pWildcard++;
					/* merken uns die Stelle */
					pWildcardLast = (char*)pWildcard;
					/* und gehen in den Such-Zustand */
					stt = WCS_SEARCH;
				}
				/* b) Es eine Abweichung gibt */
				else if (*pWildcard != *pString)
				{
					/* Dann wars das */
					stt = WCS_MISMATCH;
				}
				/* c) Beide Strings aufhören */
				else if ('\0' == *pWildcard && '\0' == *pString)
				{
					/* Dann hats gepasst */
					stt = WCS_MATCH;
				}
				else
				{
					pWildcard++;
					pString++;
				}
				break;
			/* Hier suchen wir das erste Zeichen hinter dem * im String */
			case WCS_SEARCH:
				pString = strchr(pString, *pWildcard);
				/* strchr findet keine \0 */
				if ('\0' == pWildcard)
				{
					/* In diesem Fall hat der Suchstring mit * aufgehört und passt auf alles */
					stt = WCS_MATCH;
				}
				/* gibts nicht? */
				else if (NULL == pString)
					/* Dann wars das */
					stt = WCS_MISMATCH;
				else
					/* ansonsten vergleichen wir ab hier */
					stt = WCS_SEARCH2;
				break;
			case WCS_SEARCH2:
				/* wie oben */
				if ('*' == *pWildcard)
				{
					pWildcard++;
					pWildcardLast = (char*)pWildcard;
					stt = WCS_SEARCH;
				}
				/* wie oben */
				else if ('\0' == *pString && '\0' == *pWildcard)
				{
					stt = WCS_MATCH;
				}
				/* hört der String auf, aber der Suchstring nicht... */
				else if ('\0' == *pString && '\0' != *pWildcard)
				{
					/* ...fliegen wir raus */
					stt = WCS_MISMATCH;
				}
				/* gibt es einen Unterschied */
				else if (*pWildcard != *pString)
				{
					/* Suchen wir ab dem nächsten Vorkommen */
					pWildcard = pWildcardLast;
					stt = WCS_SEARCH;
				}
				else
				{
					pWildcard++;
					pString++;
				}
				break;
			default:
				/* hier sollten wir nie hinkommen */
				stt = WCS_MISMATCH;
				break;
		} /* switch */
	} /* while */

	return (WCS_MATCH == stt);
} /* wildcardfit */

