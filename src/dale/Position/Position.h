#ifndef DALE_POSITION
#define DALE_POSITION

namespace dale {
/*! Position

    Represents a position in the code (line and column numbers).
*/
class Position {
    private:
    /*! The line number. */
    int line_number;
    /*! The column number. */
    int column_number;

    public:
    Position();
    /*! Construct a new position with the given co-ordinates.
     *  @param line_number The line number.
     *  @param column_number The column number.
     */
    Position(int line_number, int column_number);
    /*! Construct a new position using an existing position.
     *  @param other The other position.
     */
    explicit Position(Position *other);
    /*! Get the line number of the position.
     */
    int getLineNumber();
    /*! Get the column number of the position.
     */
    int getColumnNumber();
    /*! Copy the details of this position to another.
     *  @param other The other position.
     */
    void copyTo(Position *p);
    /*! Set the line and column numbers for this position.
     *  @param line_number The line number.
     *  @param column_number The column number.
     */
    void setLineAndColumn(int line_number, int column_number);
    /*! Set the line and column numbers for this position to zero.
     */
    void zero();
};
}

#endif
