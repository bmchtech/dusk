#ifndef BIT_FIELD_H
#define BIT_FIELD_H

#define BIT_FIELD_PREP(x, name)   ( ((x)<<name##_SHIFT) & name##_MASK )
#define BIT_FIELD_GET(y, name)	  ( ((y) & name##_MASK)>>name##_SHIFT )
#define BIT_FIELD_SET(y, x, name) (y = ((y)&~name##_MASK) | BIT_FIELD_PREP(x,name) )

#endif