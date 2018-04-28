#include "Position.h"

namespace dale {
Position::Position() {
    line_number = 0;
    column_number = 0;
}

Position::Position(int line_number, int column_number) {
    setLineAndColumn(line_number, column_number);
}

Position::Position(Position *other) {
    line_number = other->line_number;
    column_number = other->column_number;
}

void Position::setLineAndColumn(int line_number, int column_number) {
    this->line_number = line_number;
    this->column_number = column_number;
}

int Position::getLineNumber() { return line_number; }

int Position::getColumnNumber() { return column_number; }

void Position::copyTo(Position *other) {
    other->line_number = line_number;
    other->column_number = column_number;
}

void Position::zero() {
    line_number = 0;
    column_number = 0;
}
}
