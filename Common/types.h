#ifndef TYPES_H
#define TYPES_H

#define NUM_OF_TYPES 4

enum CellType {
    Normal,
    T_type,
    B_type,
    Limfoma
};

#define NUM_OF_ATTRIBUTES 3

enum Attribute {
    INERTIA,
    ENTROPY,
    HCOMP
};

#endif // TYPES_H
