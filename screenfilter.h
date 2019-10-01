/*
 * screenfilter.h:
 *
 * Copyright (c) 2002 DecisionSoft Ltd.
 * Paul Warren (pdw) Fri Oct 25 10:25:50 2002
 *
 * RCS: $Id: screenfilter.h,v 1.1 2002/10/25 09:59:02 pdw Exp $
 */

#ifndef __SCREENFILTER_H_ /* include guard */
#define __SCREENFILTER_H_

int screen_filter_set(char* s);
int screen_filter_match(char* s);

#endif /* __SCREENFILTER_H_ */
