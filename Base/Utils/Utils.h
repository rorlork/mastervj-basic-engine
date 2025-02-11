#ifndef _UTILS_H
#define _UTILS_H

#define UAB_BUILD_GET_SET(Type, Variable) \
private: \
	Type			m_##Variable##; \
public: \
	void Set##Variable##(Type Variable) {m_##Variable##=Variable;} \
	Type Get##Variable##() const {return m_##Variable##;}

#define UAB_BUILD_GET_SET_BY_REFERENCE(Type, Variable) \
private: \
	Type			m_##Variable##; \
public: \
	void Set##Variable##(const Type &Variable) {m_##Variable##=Variable;} \
	const Type & Get##Variable##() const {return m_##Variable##;}

#define UAB_GET_PROPERTY(Type, Variable) \
	Type Get##Variable##() const {return m_##Variable##;}

#define CHECKED_RELEASE(x) if(x) {x->Release(); x = 0;}
#define CHECKED_DELETE(x) if(x) {delete x; x = 0;}
#define CHECKED_DELETE_ARRAY(x) if(x) {delete[] x; x = 0;}

#define DEG2RAD(x) (x * (float)(3.14159265358979323846 / 180.0))

#ifdef _DEBUG
#include <Windows.h>
#define DEBUG_ASSERT(expression)		\
	do {								\
		if(!(expression)				\
			&& IsDebuggerPresent())		\
			DebugBreak();				\
	} while(0)
#else
#define DEBUG_ASSERT(expression)	(void(0))
#endif

#endif