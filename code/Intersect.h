

#define T_VALID_EPSILON ( -0.0001f )
#define RAY_INTERSECT__VALID_NEG_DIST_EPSILON ( 0.025f )
#define DOT__DEGENERATE_EPSILON  ( 0.000001f )
#define DOT__COLLINEAR_EPSILON   ( 0.000001f )
#define DOT__ORTHOGONAL_EPSILON  ( 0.000001f )

struct LINE2 {
    vec2 Origin;
    vec2 Vector;
};

struct LINE2_INTERSECT {
    boo32 IsValid;
    flo32 tA;
    flo32 tB;
    vec2  P;
};

struct RAY2 {
    vec2 Origin;
    vec2 Vector;
};

struct RAY2_INTERSECT {
    boo32 WasProcessed;
    // TODO: Is this the best way to handle this!?
    // NOTE: Problem: Some collision loops need to know which entity they hit (iEntity), but BestIntersect does not track if an individual collision is new or not. BestIntersect.IsValid is set to true once, then ignored by any better collision. (ie. Meaning that the collision system only recognizes that the first valid collision is new. Any later valid collisions are not recognized as new.)
    // WasProcessed is a way for the collision system to track when it finds a new collision. Ideally, BestIntersect would have a variable IsNew, which would be set to true in the collision test functions (eg. DoesRayIntersectCircleExt()) if the collision is valid, and set to false otherwise. This would mean that every collision test would be setting the variable. Too much processing.
    // WasProcessed is only modified if a new collision is the best new collision. It's also worded this way so that the variable can be initialized to false, so I don't have to update all the collision test functions.
    // As I finish writing this, I'm not even sure that the processing would be a problem, because each valid collision test checks BestIntersect->t anyway, but whatever...
    
    boo32 IsValid;
    flo32 t;
    vec2  P;
    vec2  N;
    
    flo32 Denom;
    flo32 tMin;
    flo32 s;
};

struct PENETRATE2D {
    boo32 IsValid;
    vec2  P;
    flo32 Dist;
};