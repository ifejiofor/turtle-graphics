/*
    This is the header file for turtleGraphics.c.
    It contains user-defined macro definitions and function declaration.
*/

/*
    Macro definitions
*/
#define QUANTUM_DISTANCE 10
#define QUANTUM_ANGLE (M_PI / 2)
#define UNIT_DISTANCE 1
#define UNIT_ANGLE (M_PI / 48)
#define ACCEPTABLE_ERROR_LIMIT_FOR_ANGLES (UNIT_ANGLE / 2)
#define ONE_CYCLE (2 * M_PI)
#define RANGE_OF_HORIZONTAL_POSITIONS_WITHIN_CANVAS (30 * QUANTUM_DISTANCE)
#define RANGE_OF_VERTICAL_POSITIONS_WITHIN_CANVAS (30 * QUANTUM_DISTANCE)
#define MINIMUM_HORIZONTAL_POSITION_WITHIN_CANVAS 0
#define MAXIMUM_HORIZONTAL_POSITION_WITHIN_CANVAS RANGE_OF_HORIZONTAL_POSITIONS_WITHIN_CANVAS
#define MINIMUM_VERTICAL_POSITION_WITHIN_CANVAS 0
#define MAXIMUM_VERTICAL_POSITION_WITHIN_CANVAS RANGE_OF_VERTICAL_POSITIONS_WITHIN_CANVAS
#define INVERSE_SCALING_FACTOR_OF_TURTLE_SIZE_WITH_RESPECT_TO_WIDTH_OF_CANVAS 1000
#define RADIUS_OF_EYE_OF_TURTLE 5
#define RADIUS_OF_SHELL_OF_TURTLE 40
#define NUMBER_OF_VERTICES_OF_HEAD_OF_TURTLE 8
#define NUMBER_OF_VERTICES_OF_PEN_OF_TURTLE 5
#define NUMBER_OF_VERTICES_OF_RIGHT_FORE_LIMB_OF_TURTLE 13
#define NUMBER_OF_VERTICES_OF_LEFT_FORE_LIMB_OF_TURTLE 13
#define NUMBER_OF_VERTICES_OF_RIGHT_HIND_LIMB_OF_TURTLE 13
#define NUMBER_OF_VERTICES_OF_LEFT_HIND_LIMB_OF_TURTLE 13
#define BACKGROUND_COLOR_OF_WINDOW RGB(255, 255, 153)
#define BACKGROUND_COLOR_OF_CONTROL_AREA RGB(153, 255, 153)
#define OUTLINE_COLOR_OF_CONTROL_AREA RGB(255, 100, 100)
#define OUTLINE_COLOR_OF_BUTTON RGB(100, 100, 100)
#define OUTLINE_COLOR_OF_BUTTON_WHEN_HOVERED RGB(250, 100, 65)
#define TEXT_COLOR_OF_BUTTON_WHEN_HOVERED RGB(150, 100, 65)
#define COLOR_OF_TURTLE_SKIN RGB(255, 255, 0)
#define COLOR_OF_TURTLE_EYE RGB(255, 255, 255)
#define COLOR_OF_TURTLE_SHELL RGB(205, 205, 205)
#define COLOR_OF_TURTLE_PEN RGB(0, 0, 255)
#define BORDER_RADIUS_OF_BUTTON 5
#define TIMER_FOR_TURNING_LEFT 1
#define TIMER_FOR_TURNING_RIGHT 2
#define TIMER_FOR_MOVING_FORWARD 3
#define TIMEOUT_VALUE 50
#define TEXT_WITHIN_BUTTON_FOR_TURNING_LEFT TEXT("Turn Left")
#define TEXT_WITHIN_BUTTON_FOR_TURNING_RIGHT TEXT("Turn Right")
#define TEXT_WITHIN_BUTTON_FOR_MOVING_FORWARD TEXT("Move Forward")
#define TEXT_WITHIN_BUTTON_FOR_SETTING_PEN_DOWN TEXT("Set Pen Down")
#define TEXT_WITHIN_BUTTON_FOR_SETTING_PEN_UP TEXT("Set Pen Up")
#define BUTTON_FOR_CHANGING_PEN_STATUS_WAS_CLICKED (WM_USER)
#define BUTTON_FOR_TURNING_LEFT_WAS_CLICKED (WM_USER + 1)
#define BUTTON_FOR_TURNING_RIGHT_WAS_CLICKED (WM_USER + 2)
#define BUTTON_FOR_MOVING_FORWARD_WAS_CLICKED (WM_USER + 3)
#define TIMER_FOR_TURNING_LEFT_WAS_TRIGGERED (WM_USER + 4)
#define TIMER_FOR_TURNING_RIGHT_WAS_TRIGGERED (WM_USER + 5)
#define TIMER_FOR_MOVING_FORWARD_WAS_TRIGGERED (WM_USER + 6)
#define LEFT_ARROW_KEY_WAS_PRESSED (WM_USER + 7)
#define RIGHT_ARROW_KEY_WAS_PRESSED (WM_USER + 8)
#define UP_ARROW_KEY_WAS_PRESSED (WM_USER + 9)
#define DOWN_ARROW_KEY_WAS_PRESSED (WM_USER + 10)
#define SPACE_BAR_WAS_PRESSED (WM_USER + 11)


/*
    User-defined type definitions
*/
typedef struct {
    double r;
    double theta;
} POLARPOINT;

typedef POINT CARTESIANPOINT;

typedef struct {
    POINT start;
    POINT end;
} LINE;

typedef struct LineNode {
    LINE line;
    struct LineNode *next;
} LINENODE;

typedef enum {PEN_UP, PEN_DOWN} TURTLEPENSTATUS;

typedef enum {
    NO_BUTTON, BUTTON_FOR_TURNING_LEFT,
    BUTTON_FOR_TURNING_RIGHT,
    BUTTON_FOR_MOVING_FORWARD,
    BUTTON_FOR_CHANGING_PEN_STATUS
} BUTTONCODE;


/*
    Function definitions.
*/
void AddLineToLinkedList(LINE lineToBeAddedToLinkedList, LINENODE **startOfLinkedList);
void ChangeTurtlePenStatus(HWND handleToWindow, POINT logicalPositionOfTurtle, double directionWhereTurtleIsFacing,
    TURTLEPENSTATUS *turtlePenStatus);
void ContinueMovingTurtleForward(POINT *positionWhereTurtleIsLocated, POINT positionWhereTurtleShouldBeLocated,
    double directionWhereTurtleIsFacing, UINT *idOfCurrentlyRunningTimer, TURTLEPENSTATUS turtlePenStatus,
    LINENODE **startOfLinkedListOfLinesDrawnByTurtle, HWND handleToWindow);
void ContinueTurningTurtle(double *directionWhereTurtleIsFacing, double directionWhereTurtleShouldBeFacing,
    POINT positionWhereTurtleIsLocated, UINT *idOfCurrentlyRunningTimer, TURTLEPENSTATUS turtlePenStatus,
    HWND handleToWindow);
void ConvertFromCanvasLogicalPointToDevicePoint(POINT points[], UINT numberOfPoints, HWND handleToWindow);
void ConvertFromTurtleLogicalPointToDevicePoint(POINT points[], UINT numberOfPoints, POINT devicePositionOfTurtle,
    HWND handleToWindow);
CARTESIANPOINT ConvertToCartesianForm(POLARPOINT polarPoint);
POLARPOINT ConvertToPolarForm(CARTESIANPOINT cartesianPoint);
BOOL CursorIsOnButtonForChangingPenStatus(HWND handleToWindow, POINT positionOfCursor, TURTLEPENSTATUS turtlePenStatus);
BOOL CursorIsOnButtonForMovingForward(HWND handleToWindow, POINT positionOfCursor);
BOOL CursorIsOnButtonForTurningLeft(HWND handleToWindow, POINT positionOfCursor);
BOOL CursorIsOnButtonForTurningRight(HWND handleToWindow, POINT positionOfCursor);
void DeleteNodeFromLinkedList(LINENODE *lineNodeToBeDeleted, LINENODE **startOfLinkedList);
BOOL DirectionsAreEqual(double direction1, double direction2);
void DrawBackgroundOfControlArea(HWND handleToWindow, HDC handleToDeviceContext);
void DrawButton(HDC handleToDeviceContext, PTSTR textToBeDrawnWithinButton, RECT rectangleEncompassingButton,
    POINT positionOfCursor);
void DrawButtonForChangingPenStatus(HWND handleToWindow, HDC handleToDeviceContext, POINT positionOfCursor, TURTLEPENSTATUS turtlePenStatus);
void DrawButtonForMovingForward(HWND handleToWindow, HDC handleToDeviceContext, POINT positionOfCursor);
void DrawButtonForTurningLeft(HWND handleToWindow, HDC handleToDeviceContext, POINT positionOfCursor);
void DrawButtonForTurningRight(HWND handleToWindow, HDC handleToDeviceContext, POINT positionOfCursor);
void DrawCanvas(HWND handleToWindow, HDC handleToDeviceContext, RECT rectangleEncompassingClippingRegion);
void DrawCircle(HDC handleToDeviceContext, POINT center, UINT radius);
void DrawControlArea(HWND handleToWindow, HDC handleToDeviceContext, TURTLEPENSTATUS turtlePenStatus,
    POINT positionOfCursor, RECT rectangleEncompassingClippingRegion);
void DrawEyesOfTurtle(HDC handleToDeviceContext, double directionWhereTurtleIsFacing);
void DrawHeadOfTurtle(HDC handleToDeviceContext, double directionWhereTurtleIsFacing);
void DrawLeftForeLimbOfTurtle(HDC handleToDeviceContext, double directionWhereTurtleIsFacing);
void DrawLeftHindLimbOfTurtle(HDC handleToDeviceContext, double directionWhereTurtleIsFacing);
void DrawLimbsOfTurtle(HDC handleToDeviceContext, double directionWhereTurtleIsFacing);
void DrawLine(HDC handleToDeviceContext, POINT startPoint, POINT endPoint);
void DrawLinesDrawnByTurtle(HWND handleToWindow, HDC handleToDeviceContext,
    LINENODE *startOfLinkedListOfLinesDrawnByTurtle, RECT rectangleEncompassingClippingRegion);
void DrawPenOfTurtle(HDC handleToDeviceContext, double directionWhereTurtleIsFacing, TURTLEPENSTATUS turtlePenStatus);
void DrawRightForeLimbOfTurtle(HDC handleToDeviceContext, double directionWhereTurtleIsFacing);
void DrawRightHindLimbOfTurtle(HDC handleToDeviceContext, double directionWhereTurtleIsFacing);
void DrawShellOfTurtle(HDC handleToDeviceContext, double directionWhereTurtleIsFacing);
void DrawTurtle(HWND handleToWindow, HDC handleToDeviceContext, POINT positionOfTurtle,
    double directionWhereTurtleIsFacing, TURTLEPENSTATUS turtlePenStatus, RECT rectangleEncompassingClippingRegion); // 'positionOfTurtle' passed to this function must be in canvas logical units
void ExtendHorizontalLine(LINENODE *lineNode, POINT point);
void ExtendLine(LINENODE *lineNode, POINT point);
void ExtendVerticalLine(LINENODE *lineNode, POINT point);
UINT GetAverageCharacterWidth(HWND handleToWindow);
UINT GetBottomOffsetOfCanvas(HWND handleToWindow);
BUTTONCODE GetCodeOfButtonThatCursorIsOver(POINT positionOfCursor, HWND handleToWindow,
    TURTLEPENSTATUS turtlePenStatus);
double GetDirectionThatIsOneUnitCloserToWhereTurtleShouldBeFacing(double directionWhereTurtleIsFacing,
    double directionWhereTurtleShouldBeFacing);
double GetDirectionWhereTurtleShouldFaceAfterTurningLeft(double directionWhereTurtleIsFacing);
double GetDirectionWhereTurtleShouldFaceAfterTurningRight(double directionWhereTurtleIsFacing);
UINT GetHeightOfCanvas(HWND handleToWindow);
UINT GetHeightOfClientArea(HWND handleToWindow);
UINT GetHeightOfModerateSizedButton(HWND handleToWindow);
UINT GetLeftOffsetOfCanvas(HWND handleToWindow);
LINE GetLineAlongBottomOfCanvas(RECT rectangleEncompassingCanvas, RECT rectangleEncompassingAreaAroundCanvas);
LINE GetLineAlongLeftOfCanvas(RECT rectangleEncompassingCanvas, RECT rectangleEncompassingAreaAroundCanvas);
LINE GetLineAlongRightOfCanvas(RECT rectangleEncompassingCanvas, RECT rectangleEncompassingAreaAroundCanvas);
LINE GetLineAlongTopOfCanvas(RECT rectangleEncompassingCanvas, RECT rectangleEncompassingAreaAroundCanvas);
CARTESIANPOINT GetLogicalCenterOfShellOfTurtle(double directionWhereTurtleIsFacing);
CARTESIANPOINT *GetLogicalVerticesOfHeadOfTurtle(double directionWhereTurtleIsFacing);
CARTESIANPOINT *GetLogicalVerticesOfLeftForeLimbOfTurtle(double directionWhereTurtleIsFacing);
CARTESIANPOINT *GetLogicalVerticesOfLeftHindLimbOfTurtle(double directionWhereTurtleIsFacing);
CARTESIANPOINT *GetLogicalVerticesOfPenOfTurtle(double directionWhereTurtleIsFacing, TURTLEPENSTATUS turtlePenStatus);
CARTESIANPOINT *GetLogicalVerticesOfRightForeLimbOfTurtle(double directionWhereTurtleIsFacing);
CARTESIANPOINT *GetLogicalVerticesOfRightHindLimbOfTurtle(double directionWhereTurtleIsFacing);
UINT GetMaximumCharacterHeight(HWND handleToWindow);
UINT GetMinimumPossibleHeightOfControlArea(HWND handleToWindow);
UINT GetMinimumPossibleOffsetOfCanvas(HWND handleToWindow);
POINT GetPositionThatIsOneUnitCloserToWhereTurtleShouldBeLocated(POINT positionWhereTurtleIsLocated,
    POINT positionWhereTurtleShouldBeLocated);
POINT GetPositionWhereTurtleShouldBeAfterMovingForward(POINT positionWhereTurtleIsLocated,
    double directionWhereTurtleIsFacing);
RECT GetRectangleEncompassingAreaAroundCanvas(RECT rectangleEncompassingCanvas, HWND handleToWindow);
RECT GetRectangleEncompassingButton(BUTTONCODE buttonCode, HWND handleToWindow, TURTLEPENSTATUS turtlePenStatus);
RECT GetRectangleEncompassingButtonForChangingPenStatus(HWND handleToWindow, TURTLEPENSTATUS currentTurtlePenStatus);
RECT GetRectangleEncompassingButtonForMovingForward(HWND handleToWindow);
RECT GetRectangleEncompassingButtonForTurningLeft(HWND handleToWindow);
RECT GetRectangleEncompassingButtonForTurningRight(HWND handleToWindow);
RECT GetRectangleEncompassingCanvas(HWND handleToWindow);
RECT GetRectangleEncompassingControlArea(HWND handleToWindow);
RECT GetRectangleEncompassingHeadOfTurtle(HWND handleToWindow, POINT devicePositionOfTurtle,
    double directionWhereTurtleIsFacing);
RECT GetRectangleEncompassingLeftForeLimbOfTurtle(HWND handleToWindow, POINT devicePositionOfTurtle,
    double directionWhereTurtleIsFacing);
RECT GetRectangleEncompassingLeftHindLimbOfTurtle(HWND handleToWindow, POINT devicePositionOfTurtle,
    double directionWhereTurtleIsFacing);
RECT GetRectangleEncompassingLimbsOfTurtle(HWND handleToWindow, POINT devicePositionOfTurtle,
    double directionWhereTurtleIsFacing);
RECT GetRectangleEncompassingPenOfTurtle(HWND handleToWindow, POINT devicePositionOfTurtle,
    double directionWhereTurtleIsFacing, TURTLEPENSTATUS turtlePenStatus);
RECT GetRectangleEncompassingPoints(CONST POINT points[], UINT numberOfPoints);
RECT GetRectangleEncompassingRectangles(UINT numberOfRectangles, RECT firstRectangle, ...);
RECT GetRectangleEncompassingRightForeLimbOfTurtle(HWND handleToWindow, POINT devicePositionOfTurtle, 
    double directionWhereTurtleIsFacing);
RECT GetRectangleEncompassingRightHindLimbOfTurtle(HWND handleToWindow, POINT devicePositionOfTurtle,
    double directionWhereTurtleIsFacing);
RECT GetRectangleEncompassingTurtle(HWND handleToWindow, POINT positionOfTurtle, double directionWhereTurtleIsFacing,
    TURTLEPENSTATUS turtlePenStatus); // 'positionOfTurtle' passed to this function must be in canvas logical units
UINT GetRefinedMessage(HWND handleToWindow, UINT message, WPARAM wParam, LPARAM lParam, TURTLEPENSTATUS turtlePenStatus);
UINT GetTopOffsetOfCanvas(HWND handleToWindow);
UINT GetWidthOfCanvas(HWND handleToWindow);
UINT GetWidthOfClientArea(HWND handleToWindow);
UINT GetWidthOfModerateSizedButton(HWND handleToWindow, WORD numberOfCharactersWithinButton);
double GetSmallestPositiveEquivalentAngle(double angle);
BOOL IntersectsWithEachOther(RECT rectangle1, RECT rectangle2);
BOOL IsContainedWithinRectangle(POINT point, RECT rectangle);
BOOL IsHorizontalLine(LINE line);
BOOL IsVerticalLine(LINE line);
BOOL LineDoNotExist(LINENODE *lineNode);
BOOL LineExists(LINENODE *lineNode);
BOOL LineIntersectsWithRectangle(LINE line, RECT rectangle);
BOOL LinePassesThroughPoint(LINE line, POINT point);
void MakeTurtleToDrawLine(LINENODE **startOfLinkedListOfLinesDrawnByTurtle, POINT initialPositionOfTurtle,
    POINT finalPositionOfTurtle);
void MergeHorizontalLines(LINENODE *lineNode1, LINENODE *lineNode2, LINENODE **startOfLinkedList);
void MergeLines(LINENODE *lineNode1, LINENODE *lineNode2, LINENODE **startOfLinkedList);
void MergeVerticalLines(LINENODE *lineNode1, LINENODE *lineNode2, LINENODE **startOfLinkedList);
void PerformDownArrowKeyAction(HWND handleToWindow, double directionWhereTurtleIsFacing);
void PerformHoveringOnButtonThatCursorIsOver(HWND handleToWindow, POINT positionOfCursor,
    TURTLEPENSTATUS turtlePenStatus);
void PerformLeftArrowKeyAction(HWND handleToWindow, double directionWhereTurtleIsFacing);
void PerformRightArrowKeyAction(HWND handleToWindow, double directionWhereTurtleIsFacing);
void PerformUpArrowKeyAction(HWND handleToWindow, double directionWhereTurtleIsFacing);
void PerformPaintingOfClientArea(HWND handleToWindow, POINT logicalPositionOfTurtle,
    double directionWhereTurtleIsFacing, TURTLEPENSTATUS turtlePenStatus,
    LINENODE *startOfLinkedListOfLinesDrawnByTurtle, POINT positionOfCursor);
BOOL PositionsAreEqual(POINT position1, POINT position2);
void RotatePoints(CARTESIANPOINT points[], UINT numberOfPoints, double angleOfRotation);
LONG RoundOffToNearestWholeNumber(double number);
LINENODE *SearchForHorizontalLineThatPassesThroughPoint(POINT point, LINENODE *startOfLinkedList);
LINENODE *SearchForVerticalLineThatPassesThroughPoint(POINT point, LINENODE *startOfLinkedList);
void SetMapModeForReferencingCanvasLogicalPoints(HWND handleToWindow, HDC handleToDeviceContext);
void SetMapModeForReferencingTurtleLogicalPoints(HWND handleToWindow, HDC handleToDeviceContext,
    POINT devicePositionOfTurtle);
void StartMovingTurtleForward(POINT positionWhereTurtleIsLocated, POINT *positionWhereTurtleShouldBeLocated,
    double directionWhereTurtleIsFacing, UINT *idOfCurrentlyRunningTimer, HWND handleToWindow);
void StartTurningTurtleLeft(double directionWhereTurtleIsFacing, double *directionWhereTurtleShouldBeFacing,
    UINT *idOfCurrentlyRunningTimer, HWND handleToWindow);
void StartTurningTurtleRight(double directionWhereTurtleIsFacing, double *directionWhereTurtleShouldBeFacing,
    UINT *idOfCurrentlyRunningTimer, HWND handleToWindow);
void StopMovingTurtleForward(UINT *idOfCurrentlyRunningTimer, HWND handleToWindow);
void StopTurningTurtle(double *directionWhereTurtleIsFacing, double directionWhereTurtleShouldBeFacing,
    UINT *idOfCurrentlyRunningTimer, HWND handleToWindow);
BOOL TurtleIsNotInMotion(UINT idOfCurrentlyRunningTimer);
BOOL TurtleIsInMotion(UINT idOfCurrentlyRunningTimer);
LRESULT CALLBACK WindowProcedure(HWND handleToWindow, UINT message, WPARAM wParam, LPARAM lParam);