/*
    This is a turtle graphics program that creates a turtle that can move, rotate, and draw lines.
    This program is the GUI version of my earlier command line turtle graphics program.
*/
#include <windows.h>
#include <math.h>
#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>
#include "turtleGraphics.h"


INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PTSTR szCmdLine, INT iCmdShow)
{
    WNDCLASS windowClass;
    BOOL registrationWasSuccessful, registrationWasNotSuccessful;
    HWND handleToWindow;
    MSG messageStructure;
    const PTSTR windowClassName = TEXT("windowClass");
    const PTSTR windowName = TEXT("Ife Turtle Graphics");

    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProcedure;
    windowClass.cbWndExtra = 0;
    windowClass.cbClsExtra = 0;
    windowClass.hInstance = hInstance;
    windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = CreateSolidBrush(BACKGROUND_COLOR_OF_WINDOW);
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = windowClassName;

    registrationWasSuccessful = RegisterClass(&windowClass);
    registrationWasNotSuccessful = !registrationWasSuccessful;

    if (registrationWasNotSuccessful) {
        MessageBox(NULL, TEXT("Unable to register window class"), TEXT("Error"), MB_ICONERROR);
        return 0;
    }

    handleToWindow = CreateWindow(windowClassName, windowName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    ShowWindow(handleToWindow, iCmdShow);
    UpdateWindow(handleToWindow);

    while (GetMessage(&messageStructure, NULL, 0, 0)) {
        TranslateMessage(&messageStructure);
        DispatchMessage(&messageStructure);
    }

    return 0;
}


LRESULT CALLBACK WindowProcedure(HWND handleToWindow, UINT message, WPARAM wParam, LPARAM lParam)
{
    static POINT positionOfCursor;

    static UINT idOfCurrentlyRunningTimer;
    static TURTLEPENSTATUS turtlePenStatus = PEN_UP;

    // Pointer to linked list of LINE structures, representing lines that have already been drawn by turtle
    static LINENODE *startOfLinkedListOfLinesDrawnByTurtle = NULL;

    // Angle between direction where turtle is facing and positive x-axis. Measured in radians.
    static double directionWhereTurtleIsFacing;
    static double directionWhereTurtleShouldBeFacing;

    // Position of turtle. Measured in logical units relative to top-left corner of canvas.
    static POINT positionWhereTurtleIsLocated = {150, 150};
    static POINT positionWhereTurtleShouldBeLocated = {150, 150};

    message = GetRefinedMessage(handleToWindow, message, wParam, lParam, turtlePenStatus);

    switch (message) {
        case WM_PAINT:
            PerformPaintingOfClientArea(handleToWindow, positionWhereTurtleIsLocated, directionWhereTurtleIsFacing,
                turtlePenStatus, startOfLinkedListOfLinesDrawnByTurtle, positionOfCursor);
            return 0;
        case WM_MOUSEMOVE:
            positionOfCursor.x = LOWORD(lParam);
            positionOfCursor.y = HIWORD(lParam);
            PerformHoveringOnButtonThatCursorIsOver(handleToWindow, positionOfCursor, turtlePenStatus);
            return 0;
        case BUTTON_FOR_CHANGING_PEN_STATUS_WAS_CLICKED:
            ChangeTurtlePenStatus(handleToWindow, positionWhereTurtleIsLocated, directionWhereTurtleIsFacing,
                &turtlePenStatus);
            return 0;
        case BUTTON_FOR_TURNING_LEFT_WAS_CLICKED:
            StartTurningTurtleLeft(directionWhereTurtleIsFacing, &directionWhereTurtleShouldBeFacing,
                &idOfCurrentlyRunningTimer, handleToWindow);
            return 0;
        case BUTTON_FOR_TURNING_RIGHT_WAS_CLICKED:
            StartTurningTurtleRight(directionWhereTurtleIsFacing, &directionWhereTurtleShouldBeFacing,
                &idOfCurrentlyRunningTimer, handleToWindow);
            return 0;
        case TIMER_FOR_TURNING_LEFT_WAS_TRIGGERED:
        case TIMER_FOR_TURNING_RIGHT_WAS_TRIGGERED:
            ContinueTurningTurtle(&directionWhereTurtleIsFacing, directionWhereTurtleShouldBeFacing,
                positionWhereTurtleIsLocated, &idOfCurrentlyRunningTimer, turtlePenStatus, handleToWindow);
            return 0;
        case BUTTON_FOR_MOVING_FORWARD_WAS_CLICKED:
            StartMovingTurtleForward(positionWhereTurtleIsLocated, &positionWhereTurtleShouldBeLocated,
                directionWhereTurtleIsFacing, &idOfCurrentlyRunningTimer, handleToWindow);
            return 0;
        case TIMER_FOR_MOVING_FORWARD_WAS_TRIGGERED:
            ContinueMovingTurtleForward(&positionWhereTurtleIsLocated, positionWhereTurtleShouldBeLocated,
                directionWhereTurtleIsFacing, &idOfCurrentlyRunningTimer, turtlePenStatus, &startOfLinkedListOfLinesDrawnByTurtle,
                handleToWindow);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(handleToWindow, message, wParam, lParam);
}


UINT GetRefinedMessage(HWND handleToWindow, UINT message, WPARAM wParam, LPARAM lParam, TURTLEPENSTATUS turtlePenStatus)
{
    if (message == WM_LBUTTONDOWN) {
        POINT positionOfCursorWhenClickOccured = {LOWORD(lParam), HIWORD(lParam)};

        if (CursorIsOnButtonForChangingPenStatus(handleToWindow, positionOfCursorWhenClickOccured, turtlePenStatus)) {
            return BUTTON_FOR_CHANGING_PEN_STATUS_WAS_CLICKED;
        }
        else if (CursorIsOnButtonForTurningLeft(handleToWindow, positionOfCursorWhenClickOccured)) {
            return BUTTON_FOR_TURNING_LEFT_WAS_CLICKED;
        }
        else if (CursorIsOnButtonForTurningRight(handleToWindow, positionOfCursorWhenClickOccured)) {
            return BUTTON_FOR_TURNING_RIGHT_WAS_CLICKED;
        }
        else if (CursorIsOnButtonForMovingForward(handleToWindow, positionOfCursorWhenClickOccured)) {
            return BUTTON_FOR_MOVING_FORWARD_WAS_CLICKED;
        }
    }

    if (message == WM_TIMER) {
        if (wParam == TIMER_FOR_TURNING_LEFT) {
            return TIMER_FOR_TURNING_LEFT_WAS_TRIGGERED;
        }
        else if (wParam == TIMER_FOR_TURNING_RIGHT) {
            return TIMER_FOR_TURNING_RIGHT_WAS_TRIGGERED;
        }
        else if (wParam == TIMER_FOR_MOVING_FORWARD) {
            return TIMER_FOR_MOVING_FORWARD_WAS_TRIGGERED;
        }
    }

    return message;
}


void ChangeTurtlePenStatus(HWND handleToWindow, POINT logicalPositionOfTurtle, double directionWhereTurtleIsFacing,
    TURTLEPENSTATUS *turtlePenStatus)
{
    RECT rectangleEncompassingAreaThatNeedsToBeRedrawn;
    RECT rectangleEncompassingButtonInitially, rectangleEncompassingButtonSubsequently;

    rectangleEncompassingButtonInitially =
        GetRectangleEncompassingButtonForChangingPenStatus(handleToWindow, *turtlePenStatus);

    *turtlePenStatus = (*turtlePenStatus == PEN_UP) ? PEN_DOWN : PEN_UP;

    rectangleEncompassingButtonSubsequently =
        GetRectangleEncompassingButtonForChangingPenStatus(handleToWindow, *turtlePenStatus);

    rectangleEncompassingAreaThatNeedsToBeRedrawn = GetRectangleEncompassingTurtle(handleToWindow,
        logicalPositionOfTurtle, directionWhereTurtleIsFacing, *turtlePenStatus);
    InvalidateRect(handleToWindow, &rectangleEncompassingAreaThatNeedsToBeRedrawn, TRUE);
    UpdateWindow(handleToWindow);

    rectangleEncompassingAreaThatNeedsToBeRedrawn = GetRectangleEncompassingRectangles(2,
        rectangleEncompassingButtonInitially, rectangleEncompassingButtonSubsequently);
    InvalidateRect(handleToWindow, &rectangleEncompassingAreaThatNeedsToBeRedrawn, TRUE);
    UpdateWindow(handleToWindow);
}


void StartTurningTurtleLeft(double directionWhereTurtleIsFacing, double *directionWhereTurtleShouldBeFacing,
    UINT *idOfCurrentlyRunningTimer, HWND handleToWindow)
{
    if (TurtleIsNotInMotion(*idOfCurrentlyRunningTimer)) {
        *directionWhereTurtleShouldBeFacing =
            GetDirectionWhereTurtleShouldFaceAfterTurningLeft(directionWhereTurtleIsFacing);
        *idOfCurrentlyRunningTimer = SetTimer(handleToWindow, TIMER_FOR_TURNING_LEFT, TIMEOUT_VALUE, NULL);
    }
}


void StartTurningTurtleRight(double directionWhereTurtleIsFacing, double *directionWhereTurtleShouldBeFacing,
    UINT *idOfCurrentlyRunningTimer, HWND handleToWindow)
{
    if (TurtleIsNotInMotion(*idOfCurrentlyRunningTimer)) {
        *directionWhereTurtleShouldBeFacing =
            GetDirectionWhereTurtleShouldFaceAfterTurningRight(directionWhereTurtleIsFacing);
        *idOfCurrentlyRunningTimer = SetTimer(handleToWindow, TIMER_FOR_TURNING_RIGHT, TIMEOUT_VALUE, NULL);
    }
}


void StartMovingTurtleForward(POINT positionWhereTurtleIsLocated, POINT *positionWhereTurtleShouldBeLocated,
    double directionWhereTurtleIsFacing, UINT *idOfCurrentlyRunningTimer, HWND handleToWindow)
{
    if (TurtleIsNotInMotion(*idOfCurrentlyRunningTimer)) {
        *positionWhereTurtleShouldBeLocated = GetPositionWhereTurtleShouldBeAfterMovingForward(
            positionWhereTurtleIsLocated, directionWhereTurtleIsFacing);

        if (PositionsAreEqual(positionWhereTurtleIsLocated, *positionWhereTurtleShouldBeLocated)) {
            return;
        }

        *idOfCurrentlyRunningTimer = SetTimer(handleToWindow, TIMER_FOR_MOVING_FORWARD, TIMEOUT_VALUE, NULL);
    }
}


void ContinueTurningTurtle(double *directionWhereTurtleIsFacing, double directionWhereTurtleShouldBeFacing,
    POINT positionWhereTurtleIsLocated, UINT *idOfCurrentlyRunningTimer, TURTLEPENSTATUS turtlePenStatus,
    HWND handleToWindow)
{
    RECT rectangleEncompassingTurtleInitially, rectangleEncompassingTurtleSubsequently;
    RECT rectangleEncompassingAreaThatNeedsToBeRedrawn;

    rectangleEncompassingTurtleInitially = GetRectangleEncompassingTurtle(handleToWindow,
        positionWhereTurtleIsLocated, *directionWhereTurtleIsFacing, turtlePenStatus);
    *directionWhereTurtleIsFacing = GetDirectionThatIsOneUnitCloserToWhereTurtleShouldBeFacing(
        *directionWhereTurtleIsFacing, directionWhereTurtleShouldBeFacing);

    if (DirectionsAreEqual(*directionWhereTurtleIsFacing, directionWhereTurtleShouldBeFacing)) {
        StopTurningTurtle(directionWhereTurtleIsFacing, directionWhereTurtleShouldBeFacing,
            idOfCurrentlyRunningTimer, handleToWindow);
    }

    rectangleEncompassingTurtleSubsequently = GetRectangleEncompassingTurtle(handleToWindow,
        positionWhereTurtleIsLocated, *directionWhereTurtleIsFacing, turtlePenStatus);
    rectangleEncompassingAreaThatNeedsToBeRedrawn = GetRectangleEncompassingRectangles(2,
        rectangleEncompassingTurtleInitially, rectangleEncompassingTurtleSubsequently);
    InvalidateRect(handleToWindow, &rectangleEncompassingAreaThatNeedsToBeRedrawn, TRUE);
    UpdateWindow(handleToWindow);
}


void StopTurningTurtle(double *directionWhereTurtleIsFacing, double directionWhereTurtleShouldBeFacing,
    UINT *idOfCurrentlyRunningTimer, HWND handleToWindow)
{
    *directionWhereTurtleIsFacing = GetSmallestPositiveEquivalentAngle(directionWhereTurtleShouldBeFacing);
    KillTimer(handleToWindow, *idOfCurrentlyRunningTimer);
    *idOfCurrentlyRunningTimer = 0;
}


void ContinueMovingTurtleForward(POINT *positionWhereTurtleIsLocated, POINT positionWhereTurtleShouldBeLocated,
    double directionWhereTurtleIsFacing, UINT *idOfCurrentlyRunningTimer, TURTLEPENSTATUS turtlePenStatus,
    LINENODE **startOfLinkedListOfLinesDrawnByTurtle, HWND handleToWindow)
{
    RECT rectangleEncompassingTurtleInitially, rectangleEncompassingTurtleSubsequently;
    RECT rectangleEncompassingAreaThatNeedsToBeRedrawn;
    POINT initialPosition, finalPosition;

    rectangleEncompassingTurtleInitially = GetRectangleEncompassingTurtle(handleToWindow,
        *positionWhereTurtleIsLocated, directionWhereTurtleIsFacing, turtlePenStatus);

    initialPosition = *positionWhereTurtleIsLocated;
    *positionWhereTurtleIsLocated = GetPositionThatIsOneUnitCloserToWhereTurtleShouldBeLocated(
        *positionWhereTurtleIsLocated, positionWhereTurtleShouldBeLocated);
    finalPosition = *positionWhereTurtleIsLocated;

    if (PositionsAreEqual(*positionWhereTurtleIsLocated, positionWhereTurtleShouldBeLocated)) {
        StopMovingTurtleForward(idOfCurrentlyRunningTimer, handleToWindow);
    }

    if (turtlePenStatus == PEN_DOWN) {
        MakeTurtleToDrawLine(startOfLinkedListOfLinesDrawnByTurtle, initialPosition, finalPosition);
    }

    rectangleEncompassingTurtleSubsequently = GetRectangleEncompassingTurtle(handleToWindow,
        *positionWhereTurtleIsLocated, directionWhereTurtleIsFacing, turtlePenStatus);
    rectangleEncompassingAreaThatNeedsToBeRedrawn = GetRectangleEncompassingRectangles(2,
        rectangleEncompassingTurtleInitially, rectangleEncompassingTurtleSubsequently);
    InvalidateRect(handleToWindow, &rectangleEncompassingAreaThatNeedsToBeRedrawn, TRUE);
    UpdateWindow(handleToWindow);
}


void StopMovingTurtleForward(UINT *idOfCurrentlyRunningTimer, HWND handleToWindow)
{
    KillTimer(handleToWindow, *idOfCurrentlyRunningTimer);
    *idOfCurrentlyRunningTimer = 0;
}


double GetDirectionWhereTurtleShouldFaceAfterTurningLeft(double directionWhereTurtleIsFacing)
{
    return directionWhereTurtleIsFacing + QUANTUM_ANGLE;
}


double GetDirectionWhereTurtleShouldFaceAfterTurningRight(double directionWhereTurtleIsFacing)
{
    return directionWhereTurtleIsFacing - QUANTUM_ANGLE;
}


POINT GetPositionWhereTurtleShouldBeAfterMovingForward(POINT positionWhereTurtleIsLocated,
    double directionWhereTurtleIsFacing)
{
    POINT positionWhereTurtleShouldBeLocated;
    BOOL userIsAttemptingToMoveTurtleOutOfCanvas = FALSE;

    positionWhereTurtleShouldBeLocated.x = positionWhereTurtleIsLocated.x + (QUANTUM_DISTANCE *
        RoundOffToNearestWholeNumber(cos(directionWhereTurtleIsFacing)));
    positionWhereTurtleShouldBeLocated.y = positionWhereTurtleIsLocated.y - (QUANTUM_DISTANCE *
        RoundOffToNearestWholeNumber(sin(directionWhereTurtleIsFacing)));

    if (positionWhereTurtleShouldBeLocated.x < MINIMUM_HORIZONTAL_POSITION_WITHIN_CANVAS) {
        userIsAttemptingToMoveTurtleOutOfCanvas = TRUE;
        positionWhereTurtleShouldBeLocated.x = MINIMUM_HORIZONTAL_POSITION_WITHIN_CANVAS;
    }
    else if (positionWhereTurtleShouldBeLocated.x > MAXIMUM_HORIZONTAL_POSITION_WITHIN_CANVAS) {
        userIsAttemptingToMoveTurtleOutOfCanvas = TRUE;
        positionWhereTurtleShouldBeLocated.x = MAXIMUM_HORIZONTAL_POSITION_WITHIN_CANVAS;
    }

    if (positionWhereTurtleShouldBeLocated.y < MINIMUM_VERTICAL_POSITION_WITHIN_CANVAS) {
        userIsAttemptingToMoveTurtleOutOfCanvas = TRUE;
        positionWhereTurtleShouldBeLocated.y = MINIMUM_VERTICAL_POSITION_WITHIN_CANVAS;
    }
    else if (positionWhereTurtleShouldBeLocated.y > MAXIMUM_VERTICAL_POSITION_WITHIN_CANVAS) {
        userIsAttemptingToMoveTurtleOutOfCanvas = TRUE;
        positionWhereTurtleShouldBeLocated.y = MAXIMUM_VERTICAL_POSITION_WITHIN_CANVAS;
    }

    if (userIsAttemptingToMoveTurtleOutOfCanvas) {
        const PTSTR titleOfErrorMessage = TEXT("Invalid Move");
        const PTSTR bodyOfErrorMessage = TEXT("Invalid Move: You are attempting to move the turtle out of canvas.");
        MessageBox(NULL, bodyOfErrorMessage, titleOfErrorMessage, MB_ICONINFORMATION);
    }

    return positionWhereTurtleShouldBeLocated;
}


double GetDirectionThatIsOneUnitCloserToWhereTurtleShouldBeFacing(double directionWhereTurtleIsFacing,
    double directionWhereTurtleShouldBeFacing)
{
    double directionThatIsOneUnitCloser;

    if (directionWhereTurtleShouldBeFacing > directionWhereTurtleIsFacing + UNIT_ANGLE) {
        directionThatIsOneUnitCloser = directionWhereTurtleIsFacing + UNIT_ANGLE;
    }
    else if (directionWhereTurtleShouldBeFacing < directionWhereTurtleIsFacing - UNIT_ANGLE) {
        directionThatIsOneUnitCloser = directionWhereTurtleIsFacing - UNIT_ANGLE;
    }
    else {
        directionThatIsOneUnitCloser = directionWhereTurtleShouldBeFacing;
    }

    return directionThatIsOneUnitCloser;
}


POINT GetPositionThatIsOneUnitCloserToWhereTurtleShouldBeLocated(POINT positionWhereTurtleIsLocated,
    POINT positionWhereTurtleShouldBeLocated)
{
    POINT positionThatIsOneUnitCloser;

    if (positionWhereTurtleShouldBeLocated.x > positionWhereTurtleIsLocated.x) {
        positionThatIsOneUnitCloser.x = positionWhereTurtleIsLocated.x + UNIT_DISTANCE;
    }
    else if (positionWhereTurtleShouldBeLocated.x < positionWhereTurtleIsLocated.x) {
        positionThatIsOneUnitCloser.x = positionWhereTurtleIsLocated.x - UNIT_DISTANCE;
    }
    else {
        positionThatIsOneUnitCloser.x = positionWhereTurtleShouldBeLocated.x;
    }

    if (positionWhereTurtleShouldBeLocated.y > positionWhereTurtleIsLocated.y) {
        positionThatIsOneUnitCloser.y = positionWhereTurtleIsLocated.y + UNIT_DISTANCE;
    }
    else if (positionWhereTurtleShouldBeLocated.y < positionWhereTurtleIsLocated.y) {
        positionThatIsOneUnitCloser.y = positionWhereTurtleIsLocated.y - UNIT_DISTANCE;
    }
    else {
        positionThatIsOneUnitCloser.y = positionWhereTurtleShouldBeLocated.y;
    }

    return positionThatIsOneUnitCloser;
}


void MakeTurtleToDrawLine(LINENODE **startOfLinkedListOfLinesDrawnByTurtle, POINT initialPositionOfTurtle,
    POINT finalPositionOfTurtle)
{
    LINE lineThatTurtleWantsToDraw;
    LINENODE *lineThatPassesThroughInitialPosition, *lineThatPassesThroughFinalPosition;

    lineThatTurtleWantsToDraw.start = initialPositionOfTurtle;
    lineThatTurtleWantsToDraw.end = finalPositionOfTurtle;

    if (IsHorizontalLine(lineThatTurtleWantsToDraw)) {
        lineThatPassesThroughInitialPosition = SearchForHorizontalLineThatPassesThroughPoint(initialPositionOfTurtle,
            *startOfLinkedListOfLinesDrawnByTurtle);
        lineThatPassesThroughFinalPosition = SearchForHorizontalLineThatPassesThroughPoint(finalPositionOfTurtle,
            *startOfLinkedListOfLinesDrawnByTurtle);
    }
    else {
        lineThatPassesThroughInitialPosition = SearchForVerticalLineThatPassesThroughPoint(initialPositionOfTurtle,
            *startOfLinkedListOfLinesDrawnByTurtle);
        lineThatPassesThroughFinalPosition = SearchForVerticalLineThatPassesThroughPoint(finalPositionOfTurtle,
            *startOfLinkedListOfLinesDrawnByTurtle);
    }

    if (LineDoNotExist(lineThatPassesThroughInitialPosition) && LineDoNotExist(lineThatPassesThroughFinalPosition)) {
        AddLineToLinkedList(lineThatTurtleWantsToDraw, startOfLinkedListOfLinesDrawnByTurtle);
    }
    else if (LineExists(lineThatPassesThroughInitialPosition)) {
        ExtendLine(lineThatPassesThroughInitialPosition, finalPositionOfTurtle);
    }
    else if (LineExists(lineThatPassesThroughFinalPosition)) {
        ExtendLine(lineThatPassesThroughFinalPosition, initialPositionOfTurtle);
    }

    MergeLines(lineThatPassesThroughInitialPosition, lineThatPassesThroughFinalPosition,
        startOfLinkedListOfLinesDrawnByTurtle);
}


LINENODE *SearchForHorizontalLineThatPassesThroughPoint(POINT point, LINENODE *startOfLinkedList)
{
    LINENODE *lineNode;

    for (lineNode = startOfLinkedList; lineNode != NULL; lineNode = lineNode->next) {
        if (IsHorizontalLine(lineNode->line) && LinePassesThroughPoint(lineNode->line, point)) {
            return lineNode;
        }
    }

    return NULL;
}


LINENODE *SearchForVerticalLineThatPassesThroughPoint(POINT point, LINENODE *startOfLinkedList)
{
    LINENODE *lineNode;

    for (lineNode = startOfLinkedList; lineNode != NULL; lineNode = lineNode->next) {
        if (IsVerticalLine(lineNode->line) && LinePassesThroughPoint(lineNode->line, point)) {
            return lineNode;
        }
    }

    return NULL;
}


void AddLineToLinkedList(LINE lineToBeAddedToLinkedList, LINENODE **startOfLinkedList)
{
    LINENODE *lineNode;

    lineNode = malloc(sizeof(LINENODE));
    lineNode->line = lineToBeAddedToLinkedList;
    lineNode->next = *startOfLinkedList;
    *startOfLinkedList = lineNode;
}


void ExtendLine(LINENODE *lineNode, POINT point)
{
    if (IsHorizontalLine(lineNode->line)) {
        ExtendHorizontalLine(lineNode, point);
    }
    else {
        ExtendVerticalLine(lineNode, point);
    }
}


void ExtendHorizontalLine(LINENODE *lineNode, POINT point)
{
    if (lineNode->line.start.x < lineNode->line.end.x) {
        if (point.x < lineNode->line.start.x) {
            lineNode->line.start.x = point.x;
        }
        else if (point.x > lineNode->line.end.x) {
            lineNode->line.end.x = point.x;
        }
    }
    else {
        if (point.x < lineNode->line.end.x) {
            lineNode->line.end.x = point.x;
        }
        else if (point.x > lineNode->line.start.x) {
            lineNode->line.start.x = point.x;
        }
    }
}


void ExtendVerticalLine(LINENODE *lineNode, POINT point)
{
    if (lineNode->line.start.y < lineNode->line.end.y) {
        if (point.y < lineNode->line.start.y) {
            lineNode->line.start.y = point.y;
        }
        else if (point.y > lineNode->line.end.y) {
            lineNode->line.end.y = point.y;
        }
    }
    else {
        if (point.y < lineNode->line.end.y) {
            lineNode->line.end.y = point.y;
        }
        else if (point.y > lineNode->line.start.y) {
            lineNode->line.start.y = point.y;
        }
    }
}


void MergeLines(LINENODE *lineNode1, LINENODE *lineNode2, LINENODE **startOfLinkedList)
{
    if (lineNode1 == NULL || lineNode2 == NULL || lineNode1 == lineNode2) {
        return;
    }

    if (IsHorizontalLine(lineNode1->line) && IsHorizontalLine(lineNode2->line)) {
        MergeHorizontalLines(lineNode1, lineNode2, startOfLinkedList);
    }
    else {
        MergeVerticalLines(lineNode1, lineNode2, startOfLinkedList);
    }
}


void MergeHorizontalLines(LINENODE *lineNode1, LINENODE *lineNode2, LINENODE **startOfLinkedList)
{
    LINE mergedLine = lineNode1->line;

    if (lineNode1->line.start.x == lineNode2->line.start.x) {
        mergedLine.start.x = lineNode1->line.end.x;
        mergedLine.end.x = lineNode2->line.end.x;
    }
    else if (lineNode1->line.start.x == lineNode2->line.end.x) {
        mergedLine.start.x = lineNode1->line.end.x;
        mergedLine.end.x = lineNode2->line.start.x;
    }
    else if (lineNode1->line.end.x == lineNode2->line.start.x) {
        mergedLine.start.x = lineNode1->line.start.x;
        mergedLine.end.x = lineNode2->line.end.x;
    }
    else if (lineNode1->line.end.x == lineNode2->line.end.x) {
        mergedLine.start.x = lineNode1->line.start.x;
        mergedLine.end.x = lineNode2->line.start.x;
    }

    lineNode1->line = mergedLine;
    DeleteNodeFromLinkedList(lineNode2, startOfLinkedList);
}


void MergeVerticalLines(LINENODE *lineNode1, LINENODE *lineNode2, LINENODE **startOfLinkedList)
{
    LINE mergedLine = lineNode1->line;
    
    if (lineNode1->line.start.y == lineNode2->line.start.y) {
        mergedLine.start.y = lineNode1->line.end.y;
        mergedLine.end.y = lineNode2->line.end.y;
    }
    else if (lineNode1->line.start.y == lineNode2->line.end.y) {
        mergedLine.start.y = lineNode1->line.end.y;
        mergedLine.end.y = lineNode2->line.start.y;
    }
    else if (lineNode1->line.end.y == lineNode2->line.start.y) {
        mergedLine.start.y = lineNode1->line.start.y;
        mergedLine.end.y = lineNode2->line.end.y;
    }
    else if (lineNode1->line.end.y == lineNode2->line.end.y) {
        mergedLine.start.y = lineNode1->line.start.y;
        mergedLine.end.y = lineNode2->line.start.y;
    }

    lineNode1->line = mergedLine;
    DeleteNodeFromLinkedList(lineNode2, startOfLinkedList);
}


void DeleteNodeFromLinkedList(LINENODE *lineNodeToBeDeleted, LINENODE **startOfLinkedList)
{
    LINENODE *lineNode;

    if (lineNodeToBeDeleted == *startOfLinkedList) {
        *startOfLinkedList = lineNodeToBeDeleted->next;
        free(lineNodeToBeDeleted);
        return;
    }

    for (lineNode = *startOfLinkedList; lineNode->next != NULL && lineNode->next != lineNodeToBeDeleted;
        lineNode = lineNode->next)
        ;

    if (lineNode->next != NULL) {
        lineNode->next = lineNodeToBeDeleted->next;
        free(lineNodeToBeDeleted);
    }
}


BOOL IsHorizontalLine(LINE line)
{
    return line.start.y == line.end.y;
}


BOOL IsVerticalLine(LINE line)
{
    return line.start.x == line.end.x;
}


BOOL LineDoNotExist(LINENODE *lineNode)
{
    return !LineExists(lineNode);
}


BOOL LineExists(LINENODE *lineNode)
{
    return lineNode != NULL;
}


BOOL LinePassesThroughPoint(LINE line, POINT point)
{
    if (line.start.x <= line.end.x && point.x >= line.start.x && point.x <= line.end.x &&
        line.start.y <= line.end.y && point.y >= line.start.y && point.y <= line.end.y)
    {
        return TRUE;
    }

    if (line.end.x <= line.start.x && point.x >= line.end.x && point.x <= line.start.x &&
        line.end.y <= line.start.y && point.y >= line.end.y && point.y <= line.start.y)
    {
        return TRUE;
    }

    return FALSE;
}


BOOL DirectionsAreEqual(double direction1, double direction2)
{
    return fabs(direction1 - direction2) < ACCEPTABLE_ERROR_LIMIT_FOR_ANGLES;
}


BOOL PositionsAreEqual(POINT position1, POINT position2)
{
    return position1.x == position2.x && position1.y == position2.y;
}


double GetSmallestPositiveEquivalentAngle(double angle)
{
    if (angle > ONE_CYCLE) {
        while (angle > ONE_CYCLE) {
            angle -= ONE_CYCLE;
        }
    }
    else if (angle < 0) {
        while (angle < 0) {
            angle += ONE_CYCLE;
        }
    }

    return angle;
}


BOOL TurtleIsNotInMotion(UINT idOfCurrentlyRunningTimer)
{
    return !TurtleIsInMotion(idOfCurrentlyRunningTimer);
}


BOOL TurtleIsInMotion(UINT idOfCurrentlyRunningTimer)
{
    return idOfCurrentlyRunningTimer == TIMER_FOR_TURNING_LEFT ||
        idOfCurrentlyRunningTimer == TIMER_FOR_TURNING_RIGHT ||
        idOfCurrentlyRunningTimer == TIMER_FOR_MOVING_FORWARD;
}


void PerformPaintingOfClientArea(HWND handleToWindow, POINT logicalPositionOfTurtle,
    double directionWhereTurtleIsFacing, TURTLEPENSTATUS turtlePenStatus,
    LINENODE *startOfLinkedListOfLinesDrawnByTurtle, POINT positionOfCursor)
{
    HDC handleToDeviceContext;
    PAINTSTRUCT paintStructure;
    RECT rectangleEncompassingClippingRegion;
    
    handleToDeviceContext = BeginPaint(handleToWindow, &paintStructure);
    rectangleEncompassingClippingRegion = paintStructure.rcPaint;
    DrawCanvas(handleToWindow, handleToDeviceContext, rectangleEncompassingClippingRegion);
    DrawLinesDrawnByTurtle(handleToWindow, handleToDeviceContext, startOfLinkedListOfLinesDrawnByTurtle,
        rectangleEncompassingClippingRegion);
    DrawTurtle(handleToWindow, handleToDeviceContext, logicalPositionOfTurtle, directionWhereTurtleIsFacing,
        turtlePenStatus, rectangleEncompassingClippingRegion);
    DrawControlArea(handleToWindow, handleToDeviceContext, turtlePenStatus, positionOfCursor,
        rectangleEncompassingClippingRegion);
    EndPaint(handleToWindow, &paintStructure);
}


void DrawCanvas(HWND handleToWindow, HDC handleToDeviceContext, RECT rectangleEncompassingClippingRegion)
{
    RECT rectangleEncompassingCanvas, rectangleEncompassingAreaAroundCanvas;
    LINE lineAlongLeftOfCanvas, lineAlongTopOfCanvas, lineAlongRightOfCanvas, lineAlongBottomOfCanvas;
    HPEN pen;

    rectangleEncompassingCanvas = GetRectangleEncompassingCanvas(handleToWindow);
    rectangleEncompassingAreaAroundCanvas =
        GetRectangleEncompassingAreaAroundCanvas(rectangleEncompassingCanvas, handleToWindow);

    if (IntersectsWithEachOther(rectangleEncompassingAreaAroundCanvas, rectangleEncompassingClippingRegion)) {
        pen = GetStockObject(NULL_PEN);
        SelectObject(handleToDeviceContext, pen);
        Rectangle(handleToDeviceContext, rectangleEncompassingAreaAroundCanvas.left,
            rectangleEncompassingAreaAroundCanvas.top, rectangleEncompassingAreaAroundCanvas.right,
            rectangleEncompassingAreaAroundCanvas.bottom);

        lineAlongLeftOfCanvas =
            GetLineAlongLeftOfCanvas(rectangleEncompassingCanvas, rectangleEncompassingAreaAroundCanvas);
        lineAlongTopOfCanvas =
            GetLineAlongTopOfCanvas(rectangleEncompassingCanvas, rectangleEncompassingAreaAroundCanvas);
        lineAlongRightOfCanvas =
            GetLineAlongRightOfCanvas(rectangleEncompassingCanvas, rectangleEncompassingAreaAroundCanvas);
        lineAlongBottomOfCanvas =
            GetLineAlongBottomOfCanvas(rectangleEncompassingCanvas, rectangleEncompassingAreaAroundCanvas);

        pen = GetStockObject(BLACK_PEN);
        SelectObject(handleToDeviceContext, pen);

        if (LineIntersectsWithRectangle(lineAlongLeftOfCanvas, rectangleEncompassingClippingRegion)) {
            DrawLine(handleToDeviceContext, lineAlongLeftOfCanvas.start, lineAlongLeftOfCanvas.end);
        }

        if (LineIntersectsWithRectangle(lineAlongTopOfCanvas, rectangleEncompassingClippingRegion)) {
            DrawLine(handleToDeviceContext, lineAlongTopOfCanvas.start, lineAlongTopOfCanvas.end);
        }

        if (LineIntersectsWithRectangle(lineAlongRightOfCanvas, rectangleEncompassingClippingRegion)) {
            DrawLine(handleToDeviceContext, lineAlongRightOfCanvas.start, lineAlongRightOfCanvas.end);
        }

        if (LineIntersectsWithRectangle(lineAlongBottomOfCanvas, rectangleEncompassingClippingRegion)) {
            DrawLine(handleToDeviceContext, lineAlongBottomOfCanvas.start, lineAlongBottomOfCanvas.end);
        }
    }
}


void DrawLinesDrawnByTurtle(HWND handleToWindow, HDC handleToDeviceContext,
    LINENODE *startOfLinkedListOfLinesDrawnByTurtle, RECT rectangleEncompassingClippingRegion)
{
    HPEN pen;
    LINENODE *lineNode;
    LINE currentLine;

    SaveDC(handleToDeviceContext);
    pen = CreatePen(PS_SOLID, 0, COLOR_OF_TURTLE_PEN);
    SelectObject(handleToDeviceContext, pen);

    for (lineNode = startOfLinkedListOfLinesDrawnByTurtle; lineNode != NULL; lineNode = lineNode->next) {
        currentLine = lineNode->line;
        ConvertFromCanvasLogicalPointToDevicePoint((POINT *)&currentLine, 2, handleToWindow);
        if (LineIntersectsWithRectangle(currentLine, rectangleEncompassingClippingRegion)) {
            DrawLine(handleToDeviceContext, currentLine.start, currentLine.end);
        }
    }

    RestoreDC(handleToDeviceContext, -1);
    DeleteObject(pen);
}


void DrawTurtle(HWND handleToWindow, HDC handleToDeviceContext, POINT positionOfTurtle,
    double directionWhereTurtleIsFacing, TURTLEPENSTATUS turtlePenStatus, RECT rectangleEncompassingClippingRegion)
{
    RECT rectangleEncompassingTurtle = GetRectangleEncompassingTurtle(handleToWindow, positionOfTurtle,
        directionWhereTurtleIsFacing, turtlePenStatus);
    HPEN penForTurtleSkin, penForTurtlePen;
    HBRUSH brushForTurtleSkin, brushForTurtlePen;

    if (IntersectsWithEachOther(rectangleEncompassingTurtle, rectangleEncompassingClippingRegion)) {
        penForTurtleSkin = GetStockObject(BLACK_PEN);
        brushForTurtleSkin = CreateSolidBrush(COLOR_OF_TURTLE_SKIN);
        penForTurtlePen = CreatePen(PS_SOLID, 0, COLOR_OF_TURTLE_PEN);
        brushForTurtlePen = CreateSolidBrush(COLOR_OF_TURTLE_PEN);

        ConvertFromCanvasLogicalPointToDevicePoint(&positionOfTurtle, 1, handleToWindow);
        SaveDC(handleToDeviceContext);
        SetMapModeForReferencingTurtleLogicalPoints(handleToWindow, handleToDeviceContext, positionOfTurtle);

        SelectObject(handleToDeviceContext, penForTurtleSkin);
        SelectObject(handleToDeviceContext, brushForTurtleSkin);
        DrawHeadOfTurtle(handleToDeviceContext, directionWhereTurtleIsFacing);
        DrawEyesOfTurtle(handleToDeviceContext, directionWhereTurtleIsFacing);
        DrawShellOfTurtle(handleToDeviceContext, directionWhereTurtleIsFacing);

        SelectObject(handleToDeviceContext, penForTurtlePen);
        SelectObject(handleToDeviceContext, brushForTurtlePen);
        DrawPenOfTurtle(handleToDeviceContext, directionWhereTurtleIsFacing, turtlePenStatus);

        SelectObject(handleToDeviceContext, penForTurtleSkin);
        SelectObject(handleToDeviceContext, brushForTurtleSkin);
        DrawLimbsOfTurtle(handleToDeviceContext, directionWhereTurtleIsFacing);

        RestoreDC(handleToDeviceContext, -1);
        DeleteObject(brushForTurtleSkin);
        DeleteObject(penForTurtlePen);
        DeleteObject(brushForTurtlePen);
    }
}


void DrawHeadOfTurtle(HDC handleToDeviceContext, double directionWhereTurtleIsFacing)
{
    CARTESIANPOINT *logicalVertices = GetLogicalVerticesOfHeadOfTurtle(directionWhereTurtleIsFacing);
    Polygon(handleToDeviceContext, logicalVertices, NUMBER_OF_VERTICES_OF_HEAD_OF_TURTLE);
}


void DrawEyesOfTurtle(HDC handleToDeviceContext, double directionWhereTurtleIsFacing)
{
    CARTESIANPOINT logicalCenterOfRightEye = {-35, 80};
    CARTESIANPOINT logicalCenterOfLeftEye = {-35, 100};

    RotatePoints(&logicalCenterOfRightEye, 1, directionWhereTurtleIsFacing);
    RotatePoints(&logicalCenterOfLeftEye, 1, directionWhereTurtleIsFacing);
    DrawCircle(handleToDeviceContext, logicalCenterOfRightEye, RADIUS_OF_EYE_OF_TURTLE);
    DrawCircle(handleToDeviceContext, logicalCenterOfLeftEye, RADIUS_OF_EYE_OF_TURTLE);
}


void DrawShellOfTurtle(HDC handleToDeviceContext, double directionWhereTurtleIsFacing)
{
    CARTESIANPOINT logicalCenter = GetLogicalCenterOfShellOfTurtle(directionWhereTurtleIsFacing);
    DrawCircle(handleToDeviceContext, logicalCenter, RADIUS_OF_SHELL_OF_TURTLE);
}


void DrawPenOfTurtle(HDC handleToDeviceContext, double directionWhereTurtleIsFacing, TURTLEPENSTATUS turtlePenStatus)
{
    CARTESIANPOINT *logicalVertices = GetLogicalVerticesOfPenOfTurtle(directionWhereTurtleIsFacing, turtlePenStatus);
    Polygon(handleToDeviceContext, logicalVertices, NUMBER_OF_VERTICES_OF_PEN_OF_TURTLE);
}


void DrawLimbsOfTurtle(HDC handleToDeviceContext, double directionWhereTurtleIsFacing)
{
    DrawRightForeLimbOfTurtle(handleToDeviceContext, directionWhereTurtleIsFacing);
    DrawLeftForeLimbOfTurtle(handleToDeviceContext, directionWhereTurtleIsFacing);
    DrawRightHindLimbOfTurtle(handleToDeviceContext, directionWhereTurtleIsFacing);
    DrawLeftHindLimbOfTurtle(handleToDeviceContext, directionWhereTurtleIsFacing);
}


void DrawRightForeLimbOfTurtle(HDC handleToDeviceContext, double directionWhereTurtleIsFacing)
{
    CARTESIANPOINT *logicalVertices = GetLogicalVerticesOfRightForeLimbOfTurtle(directionWhereTurtleIsFacing);
    Polygon(handleToDeviceContext, logicalVertices, NUMBER_OF_VERTICES_OF_RIGHT_FORE_LIMB_OF_TURTLE);
}


void DrawLeftForeLimbOfTurtle(HDC handleToDeviceContext, double directionWhereTurtleIsFacing)
{
    CARTESIANPOINT *logicalVertices = GetLogicalVerticesOfLeftForeLimbOfTurtle(directionWhereTurtleIsFacing);
    Polygon(handleToDeviceContext, logicalVertices, NUMBER_OF_VERTICES_OF_LEFT_FORE_LIMB_OF_TURTLE);
}


void DrawRightHindLimbOfTurtle(HDC handleToDeviceContext, double directionWhereTurtleIsFacing)
{
    CARTESIANPOINT *logicalVertices = GetLogicalVerticesOfRightHindLimbOfTurtle(directionWhereTurtleIsFacing);
    Polygon(handleToDeviceContext, logicalVertices, NUMBER_OF_VERTICES_OF_RIGHT_HIND_LIMB_OF_TURTLE);
}


void DrawLeftHindLimbOfTurtle(HDC handleToDeviceContext, double directionWhereTurtleIsFacing)
{
    CARTESIANPOINT *logicalVertices = GetLogicalVerticesOfLeftHindLimbOfTurtle(directionWhereTurtleIsFacing);
    Polygon(handleToDeviceContext, logicalVertices, NUMBER_OF_VERTICES_OF_LEFT_HIND_LIMB_OF_TURTLE);
}


void DrawControlArea(HWND handleToWindow, HDC handleToDeviceContext, TURTLEPENSTATUS turtlePenStatus,
    POINT positionOfCursor, RECT rectangleEncompassingClippingRegion)
{
    RECT rectangleEncompassingControlArea;
    RECT rectangleEncompassingButtonForTurningLeft, rectangleEncompassingButtonForTurningRight;
    RECT rectangleEncompassingButtonForMovingForward, rectangleEncompassingButtonForChangingPenStatus;

    rectangleEncompassingControlArea = GetRectangleEncompassingControlArea(handleToWindow);

    if (IntersectsWithEachOther(rectangleEncompassingControlArea, rectangleEncompassingClippingRegion)) {
        DrawBackgroundOfControlArea(handleToWindow, handleToDeviceContext);

        rectangleEncompassingButtonForTurningLeft = GetRectangleEncompassingButtonForTurningLeft(handleToWindow);
        rectangleEncompassingButtonForTurningRight = GetRectangleEncompassingButtonForTurningRight(handleToWindow);
        rectangleEncompassingButtonForMovingForward = GetRectangleEncompassingButtonForMovingForward(handleToWindow);
        rectangleEncompassingButtonForChangingPenStatus =
            GetRectangleEncompassingButtonForChangingPenStatus(handleToWindow, turtlePenStatus);

        if (IntersectsWithEachOther(rectangleEncompassingButtonForTurningLeft, rectangleEncompassingClippingRegion)) {
            DrawButtonForTurningLeft(handleToWindow, handleToDeviceContext, positionOfCursor);
        }

        if (IntersectsWithEachOther(rectangleEncompassingButtonForTurningRight, rectangleEncompassingClippingRegion)) {
            DrawButtonForTurningRight(handleToWindow, handleToDeviceContext, positionOfCursor);
        }

        if (IntersectsWithEachOther(rectangleEncompassingButtonForMovingForward, rectangleEncompassingClippingRegion)) {
            DrawButtonForMovingForward(handleToWindow, handleToDeviceContext, positionOfCursor);
        }

        if (IntersectsWithEachOther(rectangleEncompassingButtonForChangingPenStatus, rectangleEncompassingClippingRegion)) {
            DrawButtonForChangingPenStatus(handleToWindow, handleToDeviceContext, positionOfCursor, turtlePenStatus);
        }
    }
}


void DrawBackgroundOfControlArea(HWND handleToWindow, HDC handleToDeviceContext)
{
    HBRUSH brush;
    HPEN pen1, pen2;
    RECT rectangleEncompassingControlArea;
    POINT topLeftOfControlArea, topRightOfControlArea;

    rectangleEncompassingControlArea = GetRectangleEncompassingControlArea(handleToWindow);
    topLeftOfControlArea.y = rectangleEncompassingControlArea.top;
    topLeftOfControlArea.x = rectangleEncompassingControlArea.left;
    topRightOfControlArea.y = rectangleEncompassingControlArea.top;
    topRightOfControlArea.x = rectangleEncompassingControlArea.right;

    SaveDC(handleToDeviceContext);
    brush = CreateSolidBrush(BACKGROUND_COLOR_OF_CONTROL_AREA);
    pen1 = GetStockObject(NULL_PEN);
    pen2 = CreatePen(PS_SOLID, 2, OUTLINE_COLOR_OF_CONTROL_AREA);
    SelectObject(handleToDeviceContext, brush);
    SelectObject(handleToDeviceContext, pen1);
    Rectangle(handleToDeviceContext, rectangleEncompassingControlArea.left, rectangleEncompassingControlArea.top,
        rectangleEncompassingControlArea.right, rectangleEncompassingControlArea.bottom);
    SelectObject(handleToDeviceContext, pen2);
    DrawLine(handleToDeviceContext, topLeftOfControlArea, topRightOfControlArea);
    RestoreDC(handleToDeviceContext, -1);
    DeleteObject(brush);
    DeleteObject(pen2);
}


void DrawButtonForTurningLeft(HWND handleToWindow, HDC handleToDeviceContext, POINT positionOfCursor)
{
    RECT rectangle = GetRectangleEncompassingButtonForTurningLeft(handleToWindow);
    DrawButton(handleToDeviceContext, TEXT_WITHIN_BUTTON_FOR_TURNING_LEFT, rectangle, positionOfCursor);
}


void DrawButtonForTurningRight(HWND handleToWindow, HDC handleToDeviceContext, POINT positionOfCursor)
{
    RECT rectangle = GetRectangleEncompassingButtonForTurningRight(handleToWindow);
    DrawButton(handleToDeviceContext, TEXT_WITHIN_BUTTON_FOR_TURNING_RIGHT, rectangle, positionOfCursor);
}


void DrawButtonForMovingForward(HWND handleToWindow, HDC handleToDeviceContext, POINT positionOfCursor)
{
    RECT rectangle = GetRectangleEncompassingButtonForMovingForward(handleToWindow);
    DrawButton(handleToDeviceContext, TEXT_WITHIN_BUTTON_FOR_MOVING_FORWARD, rectangle, positionOfCursor);
}


void DrawButtonForChangingPenStatus(HWND handleToWindow, HDC handleToDeviceContext, POINT positionOfCursor,
    TURTLEPENSTATUS turtlePenStatus)
{
    RECT rectangle;
    PTSTR textWithinButton;

    rectangle = GetRectangleEncompassingButtonForChangingPenStatus(handleToWindow, turtlePenStatus);

    if (turtlePenStatus == PEN_UP) {
        textWithinButton = TEXT_WITHIN_BUTTON_FOR_SETTING_PEN_DOWN;
    }
    else {
        textWithinButton = TEXT_WITHIN_BUTTON_FOR_SETTING_PEN_UP;
    }

    DrawButton(handleToDeviceContext, textWithinButton, rectangle, positionOfCursor);
}


void DrawButton(HDC handleToDeviceContext, PTSTR textToBeDrawnWithinButton, RECT rectangleEncompassingButton,
    POINT positionOfCursor)
{
    HPEN pen;
    UINT widthOfButton, heightOfButton;
    POINT centerOfButton;
    UINT numberOfCharactersToBeDrawnWithinButton;

    widthOfButton = rectangleEncompassingButton.right - rectangleEncompassingButton.left;
    heightOfButton = rectangleEncompassingButton.bottom - rectangleEncompassingButton.top;
    centerOfButton.x = rectangleEncompassingButton.left + (widthOfButton / 2);
    centerOfButton.y = rectangleEncompassingButton.top + (heightOfButton / 4);
    numberOfCharactersToBeDrawnWithinButton = _tcslen(textToBeDrawnWithinButton);

    SaveDC(handleToDeviceContext);

    if (IsContainedWithinRectangle(positionOfCursor, rectangleEncompassingButton)) {
        SetTextColor(handleToDeviceContext, TEXT_COLOR_OF_BUTTON_WHEN_HOVERED);
        pen = CreatePen(PS_INSIDEFRAME, 0, OUTLINE_COLOR_OF_BUTTON_WHEN_HOVERED);
        SelectObject(handleToDeviceContext, pen);
    }
    else {
        pen = CreatePen(PS_INSIDEFRAME, 0, OUTLINE_COLOR_OF_BUTTON);
        SelectObject(handleToDeviceContext, pen);
    }

    RoundRect(handleToDeviceContext, rectangleEncompassingButton.left, rectangleEncompassingButton.top,
        rectangleEncompassingButton.right, rectangleEncompassingButton.bottom, BORDER_RADIUS_OF_BUTTON,
        BORDER_RADIUS_OF_BUTTON);
    SetTextAlign(handleToDeviceContext, TA_CENTER);
    TextOut(handleToDeviceContext, centerOfButton.x, centerOfButton.y, textToBeDrawnWithinButton,
        numberOfCharactersToBeDrawnWithinButton);
    RestoreDC(handleToDeviceContext, -1);
    DeleteObject(pen);
}


void DrawCircle(HDC handleToDeviceContext, POINT center, UINT radius)
{
    RECT rectangleEncompassingCircle;

    rectangleEncompassingCircle.left = center.x - radius;
    rectangleEncompassingCircle.top = center.y - radius;
    rectangleEncompassingCircle.right = center.x + radius;
    rectangleEncompassingCircle.bottom = center.y + radius;

    Ellipse(handleToDeviceContext, rectangleEncompassingCircle.left, rectangleEncompassingCircle.top,
        rectangleEncompassingCircle.right, rectangleEncompassingCircle.bottom);
}


void DrawLine(HDC handleToDeviceContext, POINT startPoint, POINT endPoint)
{
    POINT initialCurrentPosition;
    MoveToEx(handleToDeviceContext, startPoint.x, startPoint.y, &initialCurrentPosition);
    LineTo(handleToDeviceContext, endPoint.x, endPoint.y);
    MoveToEx(handleToDeviceContext, initialCurrentPosition.x, initialCurrentPosition.y, NULL);
}


void RotatePoints(CARTESIANPOINT points[], UINT numberOfPoints, double angleOfRotation)
{
    int i;
    POLARPOINT polarFormOfPoint;

    for (i = 0; i < numberOfPoints; i++) {
        polarFormOfPoint = ConvertToPolarForm(points[i]);
        polarFormOfPoint.theta += angleOfRotation;
        points[i] = ConvertToCartesianForm(polarFormOfPoint);
    }
}


POLARPOINT ConvertToPolarForm(CARTESIANPOINT cartesianPoint)
{
    POLARPOINT polarPoint;

    polarPoint.r = sqrt((cartesianPoint.x * cartesianPoint.x) + (cartesianPoint.y * cartesianPoint.y));
    polarPoint.theta = (cartesianPoint.x != 0) ? (atan((double)cartesianPoint.y / cartesianPoint.x) + M_PI) : (M_PI / 2);
    return polarPoint;
}


CARTESIANPOINT ConvertToCartesianForm(POLARPOINT polarPoint)
{
    CARTESIANPOINT cartesianPoint;

    cartesianPoint.x = RoundOffToNearestWholeNumber(polarPoint.r * cos(polarPoint.theta));
    cartesianPoint.y = RoundOffToNearestWholeNumber(polarPoint.r * sin(polarPoint.theta));
    return cartesianPoint;
}


LONG RoundOffToNearestWholeNumber(double number)
{
    if (number > 0) {
        return (LONG)(number + 0.5);
    }
    else {
        return (LONG)(number - 0.5);
    }
}


void ConvertFromCanvasLogicalPointToDevicePoint(POINT points[], UINT numberOfPoints, HWND handleToWindow)
{
    HDC handleToDeviceContext;

    handleToDeviceContext = GetDC(handleToWindow);
    SetMapModeForReferencingCanvasLogicalPoints(handleToWindow, handleToDeviceContext);
    LPtoDP(handleToDeviceContext, points, numberOfPoints);
    ReleaseDC(handleToWindow, handleToDeviceContext);
}


void ConvertFromTurtleLogicalPointToDevicePoint(POINT points[], UINT numberOfPoints, POINT devicePositionOfTurtle,
    HWND handleToWindow)
{
    HDC handleToDeviceContext;

    handleToDeviceContext = GetDC(handleToWindow);
    SetMapModeForReferencingTurtleLogicalPoints(handleToWindow, handleToDeviceContext, devicePositionOfTurtle);
    LPtoDP(handleToDeviceContext, points, numberOfPoints);
    ReleaseDC(handleToWindow, handleToDeviceContext);
}


void SetMapModeForReferencingCanvasLogicalPoints(HWND handleToWindow, HDC handleToDeviceContext)
{
    UINT widthOfCanvas, heightOfCanvas, leftOffsetOfCanvas, topOffsetOfCanvas;

    widthOfCanvas = GetWidthOfCanvas(handleToWindow);
    heightOfCanvas = GetHeightOfCanvas(handleToWindow);
    leftOffsetOfCanvas = GetLeftOffsetOfCanvas(handleToWindow);
    topOffsetOfCanvas = GetTopOffsetOfCanvas(handleToWindow);

    SetMapMode(handleToDeviceContext, MM_ANISOTROPIC);
    SetWindowExtEx(handleToDeviceContext, RANGE_OF_HORIZONTAL_POSITIONS_WITHIN_CANVAS,
        RANGE_OF_VERTICAL_POSITIONS_WITHIN_CANVAS, NULL);
    SetViewportExtEx(handleToDeviceContext, widthOfCanvas, heightOfCanvas, NULL);
    SetWindowOrgEx(handleToDeviceContext, 0, 0, NULL);
    SetViewportOrgEx(handleToDeviceContext, leftOffsetOfCanvas, topOffsetOfCanvas, NULL);
}


void SetMapModeForReferencingTurtleLogicalPoints(HWND handleToWindow, HDC handleToDeviceContext,
    POINT devicePositionOfTurtle)
{
    UINT widthOfCanvas;

    widthOfCanvas = GetWidthOfCanvas(handleToWindow);

    SetMapMode(handleToDeviceContext, MM_ANISOTROPIC);
    SetWindowExtEx(handleToDeviceContext, INVERSE_SCALING_FACTOR_OF_TURTLE_SIZE_WITH_RESPECT_TO_WIDTH_OF_CANVAS,
        INVERSE_SCALING_FACTOR_OF_TURTLE_SIZE_WITH_RESPECT_TO_WIDTH_OF_CANVAS, NULL);
    SetViewportExtEx(handleToDeviceContext, widthOfCanvas, -widthOfCanvas, NULL);
    SetWindowOrgEx(handleToDeviceContext, 0, 0, NULL);
    SetViewportOrgEx(handleToDeviceContext, devicePositionOfTurtle.x, devicePositionOfTurtle.y, NULL);
}


BOOL CursorIsOnButtonForChangingPenStatus(HWND handleToWindow, POINT positionOfCursor, TURTLEPENSTATUS turtlePenStatus)
{
    RECT rectangle = GetRectangleEncompassingButtonForChangingPenStatus(handleToWindow, turtlePenStatus);
    return IsContainedWithinRectangle(positionOfCursor, rectangle);
}


BOOL CursorIsOnButtonForTurningLeft(HWND handleToWindow, POINT positionOfCursor)
{
    RECT rectangle = GetRectangleEncompassingButtonForTurningLeft(handleToWindow);
    return IsContainedWithinRectangle(positionOfCursor, rectangle);
}


BOOL CursorIsOnButtonForTurningRight(HWND handleToWindow, POINT positionOfCursor)
{
    RECT rectangle = GetRectangleEncompassingButtonForTurningRight(handleToWindow);
    return IsContainedWithinRectangle(positionOfCursor, rectangle);
}


BOOL CursorIsOnButtonForMovingForward(HWND handleToWindow, POINT positionOfCursor)
{
    RECT rectangle = GetRectangleEncompassingButtonForMovingForward(handleToWindow);
    return IsContainedWithinRectangle(positionOfCursor, rectangle);
}


BOOL IsContainedWithinRectangle(POINT point, RECT rectangle)
{
    return (point.x >= rectangle.left && point.x <= rectangle.right) &&
        (point.y >= rectangle.top && point.y <= rectangle.bottom);
}


BOOL IntersectsWithEachOther(RECT rectangle1, RECT rectangle2)
{
    return rectangle1.left <= rectangle2.right && rectangle2.left <= rectangle1.right &&
        rectangle1.top <= rectangle2.bottom && rectangle2.top <= rectangle1.bottom;
}


// For now, this function works well only with horizontal and vertical lines
BOOL LineIntersectsWithRectangle(LINE line, RECT rectangle)
{
    if (line.start.x > line.end.x || line.start.y > line.end.y) {
        POINT temp = line.start;
        line.start = line.end;
        line.end = temp;
    }

    return line.start.x <= rectangle.right && rectangle.left <= line.end.x &&
        line.start.y <= rectangle.bottom && rectangle.top <= line.end.y;
}


void PerformHoveringOnButtonThatCursorIsOver(HWND handleToWindow, POINT positionOfCursor,
    TURTLEPENSTATUS turtlePenStatus)
{
    static BUTTONCODE buttonThatCursorWasPreviouslyOver = NO_BUTTON;
    BUTTONCODE buttonThatCursorIsCurrentlyOver;
    RECT rectangleEncompassingButtonThatCursorWasPreviouslyOver;
    RECT rectangleEncompassingButtonThatCursorIsCurrentlyOver;

    buttonThatCursorIsCurrentlyOver =
        GetCodeOfButtonThatCursorIsOver(positionOfCursor, handleToWindow, turtlePenStatus);

    if (buttonThatCursorWasPreviouslyOver != buttonThatCursorIsCurrentlyOver) {
        rectangleEncompassingButtonThatCursorWasPreviouslyOver =
            GetRectangleEncompassingButton(buttonThatCursorWasPreviouslyOver, handleToWindow, turtlePenStatus);
        rectangleEncompassingButtonThatCursorIsCurrentlyOver =
            GetRectangleEncompassingButton(buttonThatCursorIsCurrentlyOver, handleToWindow, turtlePenStatus);
        InvalidateRect(handleToWindow, &rectangleEncompassingButtonThatCursorWasPreviouslyOver, TRUE);
        InvalidateRect(handleToWindow, &rectangleEncompassingButtonThatCursorIsCurrentlyOver, TRUE);
        UpdateWindow(handleToWindow);
        buttonThatCursorWasPreviouslyOver = buttonThatCursorIsCurrentlyOver;
    }
}


BUTTONCODE GetCodeOfButtonThatCursorIsOver(POINT positionOfCursor, HWND handleToWindow,
    TURTLEPENSTATUS turtlePenStatus)
{
    BUTTONCODE buttonCode;

    if (CursorIsOnButtonForTurningLeft(handleToWindow, positionOfCursor)) {
        buttonCode = BUTTON_FOR_TURNING_LEFT;
    }
    else if (CursorIsOnButtonForTurningRight(handleToWindow, positionOfCursor)) {
        buttonCode = BUTTON_FOR_TURNING_RIGHT;
    }
    else if (CursorIsOnButtonForMovingForward(handleToWindow, positionOfCursor)) {
        buttonCode = BUTTON_FOR_MOVING_FORWARD;
    }
    else if (CursorIsOnButtonForChangingPenStatus(handleToWindow, positionOfCursor, turtlePenStatus)) {
        buttonCode = BUTTON_FOR_CHANGING_PEN_STATUS;
    }
    else {
        buttonCode = NO_BUTTON;
    }

    return buttonCode;
}


RECT GetRectangleEncompassingButton(BUTTONCODE buttonCode, HWND handleToWindow, TURTLEPENSTATUS turtlePenStatus)
{
    RECT rectangle = {0, 0, 0, 0};

    if (buttonCode == BUTTON_FOR_TURNING_LEFT) {
        rectangle = GetRectangleEncompassingButtonForTurningLeft(handleToWindow);
    }
    else if (buttonCode == BUTTON_FOR_TURNING_RIGHT) {
        rectangle = GetRectangleEncompassingButtonForTurningRight(handleToWindow);
    }
    else if (buttonCode == BUTTON_FOR_MOVING_FORWARD) {
        rectangle = GetRectangleEncompassingButtonForMovingForward(handleToWindow);
    }
    else if (buttonCode == BUTTON_FOR_CHANGING_PEN_STATUS) {
        rectangle = GetRectangleEncompassingButtonForChangingPenStatus(handleToWindow, turtlePenStatus);
    }

    return rectangle;
}
/*

BOOL RectangleIsNotEmpty(RECT rectangle)
{
    return rectangle.left != rectangle.right || rectangle.top != rectangle.bottom;
} */


RECT GetRectangleEncompassingCanvas(HWND handleToWindow)
{
    UINT widthOfCanvas, heightOfCanvas, leftOffsetOfCanvas, topOffsetOfCanvas;
    RECT rectangleEncompassingCanvas;

    widthOfCanvas = GetWidthOfCanvas(handleToWindow);
    heightOfCanvas = GetHeightOfCanvas(handleToWindow);
    leftOffsetOfCanvas = GetLeftOffsetOfCanvas(handleToWindow);
    topOffsetOfCanvas = GetTopOffsetOfCanvas(handleToWindow);

    rectangleEncompassingCanvas.left = leftOffsetOfCanvas;
    rectangleEncompassingCanvas.top = topOffsetOfCanvas;
    rectangleEncompassingCanvas.right = rectangleEncompassingCanvas.left + widthOfCanvas;
    rectangleEncompassingCanvas.bottom = rectangleEncompassingCanvas.top + heightOfCanvas;
    return rectangleEncompassingCanvas;
}


RECT GetRectangleEncompassingAreaAroundCanvas(RECT rectangleEncompassingCanvas, HWND handleToWindow)
{
    UINT minimumPossibleOffsetOfCanvas;
    RECT rectangleEncompassingAreaAroundCanvas;

    minimumPossibleOffsetOfCanvas = GetMinimumPossibleOffsetOfCanvas(handleToWindow);
    rectangleEncompassingAreaAroundCanvas.left = rectangleEncompassingCanvas.left - minimumPossibleOffsetOfCanvas;
    rectangleEncompassingAreaAroundCanvas.top = rectangleEncompassingCanvas.top - minimumPossibleOffsetOfCanvas;
    rectangleEncompassingAreaAroundCanvas.right = rectangleEncompassingCanvas.right + minimumPossibleOffsetOfCanvas;
    rectangleEncompassingAreaAroundCanvas.bottom = rectangleEncompassingCanvas.bottom + minimumPossibleOffsetOfCanvas;
    return rectangleEncompassingAreaAroundCanvas;
}


RECT GetRectangleEncompassingTurtle(HWND handleToWindow, POINT positionOfTurtle, double directionWhereTurtleIsFacing,
    TURTLEPENSTATUS turtlePenStatus)
{
    RECT rectangleEncompassingHeadOfTurtle, rectangleEncompassingPenOfTurtle, rectangleEncompassingLimbsOfTurtle;

    ConvertFromCanvasLogicalPointToDevicePoint(&positionOfTurtle, 1, handleToWindow);

    rectangleEncompassingHeadOfTurtle = GetRectangleEncompassingHeadOfTurtle(handleToWindow, positionOfTurtle,
        directionWhereTurtleIsFacing);
    rectangleEncompassingPenOfTurtle = GetRectangleEncompassingPenOfTurtle(handleToWindow, positionOfTurtle,
        directionWhereTurtleIsFacing, turtlePenStatus);
    rectangleEncompassingLimbsOfTurtle = GetRectangleEncompassingLimbsOfTurtle(handleToWindow, positionOfTurtle,
        directionWhereTurtleIsFacing);

    return GetRectangleEncompassingRectangles(3, rectangleEncompassingHeadOfTurtle,
        rectangleEncompassingPenOfTurtle, rectangleEncompassingLimbsOfTurtle);
}


RECT GetRectangleEncompassingHeadOfTurtle(HWND handleToWindow, POINT devicePositionOfTurtle,
    double directionWhereTurtleIsFacing)
{
    POINT *vertices = GetLogicalVerticesOfHeadOfTurtle(directionWhereTurtleIsFacing);
    ConvertFromTurtleLogicalPointToDevicePoint(vertices, NUMBER_OF_VERTICES_OF_HEAD_OF_TURTLE,
        devicePositionOfTurtle, handleToWindow);
    return GetRectangleEncompassingPoints(vertices, NUMBER_OF_VERTICES_OF_HEAD_OF_TURTLE);
}


RECT GetRectangleEncompassingPenOfTurtle(HWND handleToWindow, POINT devicePositionOfTurtle,
    double directionWhereTurtleIsFacing, TURTLEPENSTATUS turtlePenStatus)
{
    POINT *vertices = GetLogicalVerticesOfPenOfTurtle(directionWhereTurtleIsFacing, turtlePenStatus);
    ConvertFromTurtleLogicalPointToDevicePoint(vertices, NUMBER_OF_VERTICES_OF_PEN_OF_TURTLE,
        devicePositionOfTurtle, handleToWindow);
    return GetRectangleEncompassingPoints(vertices, NUMBER_OF_VERTICES_OF_PEN_OF_TURTLE);
}


RECT GetRectangleEncompassingLimbsOfTurtle(HWND handleToWindow, POINT devicePositionOfTurtle,
    double directionWhereTurtleIsFacing)
{
    RECT rectangleEncompassingRightForeLimb, rectangleEncompassingLeftForeLimb;
    RECT rectangleEncompassingRightHindLimb, rectangleEncompassingLeftHindLimb;

    rectangleEncompassingRightForeLimb = GetRectangleEncompassingRightForeLimbOfTurtle(handleToWindow,
        devicePositionOfTurtle, directionWhereTurtleIsFacing);
    rectangleEncompassingLeftForeLimb = GetRectangleEncompassingLeftForeLimbOfTurtle(handleToWindow,
        devicePositionOfTurtle, directionWhereTurtleIsFacing);
    rectangleEncompassingRightHindLimb = GetRectangleEncompassingRightHindLimbOfTurtle(handleToWindow,
        devicePositionOfTurtle, directionWhereTurtleIsFacing);
    rectangleEncompassingLeftHindLimb = GetRectangleEncompassingLeftHindLimbOfTurtle(handleToWindow,
        devicePositionOfTurtle, directionWhereTurtleIsFacing);

    return GetRectangleEncompassingRectangles(4, rectangleEncompassingRightForeLimb,
        rectangleEncompassingLeftForeLimb, rectangleEncompassingRightHindLimb, rectangleEncompassingLeftHindLimb);
}


RECT GetRectangleEncompassingRightForeLimbOfTurtle(HWND handleToWindow, POINT devicePositionOfTurtle, 
    double directionWhereTurtleIsFacing)
{
    POINT *vertices = GetLogicalVerticesOfRightForeLimbOfTurtle(directionWhereTurtleIsFacing);
    ConvertFromTurtleLogicalPointToDevicePoint(vertices, NUMBER_OF_VERTICES_OF_RIGHT_FORE_LIMB_OF_TURTLE,
        devicePositionOfTurtle, handleToWindow);
    return GetRectangleEncompassingPoints(vertices, NUMBER_OF_VERTICES_OF_RIGHT_FORE_LIMB_OF_TURTLE);
}


RECT GetRectangleEncompassingLeftForeLimbOfTurtle(HWND handleToWindow, POINT devicePositionOfTurtle,
    double directionWhereTurtleIsFacing)
{
    POINT *vertices = GetLogicalVerticesOfLeftForeLimbOfTurtle(directionWhereTurtleIsFacing);
    ConvertFromTurtleLogicalPointToDevicePoint(vertices, NUMBER_OF_VERTICES_OF_LEFT_FORE_LIMB_OF_TURTLE,
        devicePositionOfTurtle, handleToWindow);
    return GetRectangleEncompassingPoints(vertices, NUMBER_OF_VERTICES_OF_LEFT_FORE_LIMB_OF_TURTLE);
}


RECT GetRectangleEncompassingRightHindLimbOfTurtle(HWND handleToWindow, POINT devicePositionOfTurtle,
    double directionWhereTurtleIsFacing)
{
    POINT *vertices = GetLogicalVerticesOfRightHindLimbOfTurtle(directionWhereTurtleIsFacing);
    ConvertFromTurtleLogicalPointToDevicePoint(vertices, NUMBER_OF_VERTICES_OF_RIGHT_HIND_LIMB_OF_TURTLE,
        devicePositionOfTurtle, handleToWindow);
    return GetRectangleEncompassingPoints(vertices, NUMBER_OF_VERTICES_OF_RIGHT_HIND_LIMB_OF_TURTLE);
}


RECT GetRectangleEncompassingLeftHindLimbOfTurtle(HWND handleToWindow, POINT devicePositionOfTurtle,
    double directionWhereTurtleIsFacing)
{
    POINT *vertices = GetLogicalVerticesOfLeftHindLimbOfTurtle(directionWhereTurtleIsFacing);
    ConvertFromTurtleLogicalPointToDevicePoint(vertices, NUMBER_OF_VERTICES_OF_LEFT_HIND_LIMB_OF_TURTLE,
        devicePositionOfTurtle, handleToWindow);
    return GetRectangleEncompassingPoints(vertices, NUMBER_OF_VERTICES_OF_LEFT_HIND_LIMB_OF_TURTLE);
}


RECT GetRectangleEncompassingButtonForTurningLeft(HWND handleToWindow)
{
    RECT rectangleEncompassingControlArea, rectangleEncompassingButtonForTurningLeft;
    UINT widthOfControlArea, heightOfControlArea, widthOfButton, heightOfButton;
    UINT numberOfCharactersWithinButton;

    rectangleEncompassingControlArea = GetRectangleEncompassingControlArea(handleToWindow);
    widthOfControlArea = rectangleEncompassingControlArea.right - rectangleEncompassingControlArea.left;
    heightOfControlArea = rectangleEncompassingControlArea.bottom - rectangleEncompassingControlArea.top;

    numberOfCharactersWithinButton = _tcslen(TEXT_WITHIN_BUTTON_FOR_TURNING_LEFT);
    widthOfButton = GetWidthOfModerateSizedButton(handleToWindow, numberOfCharactersWithinButton);
    heightOfButton = GetHeightOfModerateSizedButton(handleToWindow);

    rectangleEncompassingButtonForTurningLeft.right = (widthOfControlArea / 2) - (widthOfControlArea / 96);
    rectangleEncompassingButtonForTurningLeft.left = rectangleEncompassingButtonForTurningLeft.right - widthOfButton;
    rectangleEncompassingButtonForTurningLeft.top = rectangleEncompassingControlArea.top + (7 * heightOfControlArea / 12);
    rectangleEncompassingButtonForTurningLeft.bottom = rectangleEncompassingButtonForTurningLeft.top + heightOfButton;
    return rectangleEncompassingButtonForTurningLeft;
}


RECT GetRectangleEncompassingButtonForTurningRight(HWND handleToWindow)
{
    RECT rectangleEncompassingControlArea, rectangleEncompassingButtonForTurningRight;
    UINT widthOfControlArea, heightOfControlArea, widthOfButton, heightOfButton;
    UINT numberOfCharactersWithinButton;
 
    rectangleEncompassingControlArea = GetRectangleEncompassingControlArea(handleToWindow);
    widthOfControlArea = rectangleEncompassingControlArea.right - rectangleEncompassingControlArea.left;
    heightOfControlArea = rectangleEncompassingControlArea.bottom - rectangleEncompassingControlArea.top;

    numberOfCharactersWithinButton = _tcslen(TEXT_WITHIN_BUTTON_FOR_TURNING_RIGHT);
    widthOfButton = GetWidthOfModerateSizedButton(handleToWindow, numberOfCharactersWithinButton);
    heightOfButton = GetHeightOfModerateSizedButton(handleToWindow);

    rectangleEncompassingButtonForTurningRight.left = (widthOfControlArea / 2) + (widthOfControlArea / 96);
    rectangleEncompassingButtonForTurningRight.right = rectangleEncompassingButtonForTurningRight.left + widthOfButton;
    rectangleEncompassingButtonForTurningRight.top = rectangleEncompassingControlArea.top + (7 * heightOfControlArea / 12);
    rectangleEncompassingButtonForTurningRight.bottom = rectangleEncompassingButtonForTurningRight.top + heightOfButton;
    return rectangleEncompassingButtonForTurningRight;
}


RECT GetRectangleEncompassingButtonForMovingForward(HWND handleToWindow)
{
    RECT rectangleEncompassingControlArea, rectangleEncompassingButtonForMovingForward;
    UINT widthOfControlArea, heightOfControlArea, widthOfButton, heightOfButton;
    UINT numberOfCharactersWithinButton;

    rectangleEncompassingControlArea = GetRectangleEncompassingControlArea(handleToWindow);
    widthOfControlArea = rectangleEncompassingControlArea.right - rectangleEncompassingControlArea.left;
    heightOfControlArea = rectangleEncompassingControlArea.bottom - rectangleEncompassingControlArea.top;

    numberOfCharactersWithinButton = _tcslen(TEXT_WITHIN_BUTTON_FOR_MOVING_FORWARD);
    widthOfButton = GetWidthOfModerateSizedButton(handleToWindow, numberOfCharactersWithinButton);
    heightOfButton = GetHeightOfModerateSizedButton(handleToWindow);

    rectangleEncompassingButtonForMovingForward.left = (widthOfControlArea / 2) - (widthOfButton / 2);
    rectangleEncompassingButtonForMovingForward.right = rectangleEncompassingButtonForMovingForward.left + widthOfButton;
    rectangleEncompassingButtonForMovingForward.top = rectangleEncompassingControlArea.top + (heightOfControlArea / 12);
    rectangleEncompassingButtonForMovingForward.bottom = rectangleEncompassingButtonForMovingForward.top + heightOfButton;
    return rectangleEncompassingButtonForMovingForward;
}


RECT GetRectangleEncompassingButtonForChangingPenStatus(HWND handleToWindow, TURTLEPENSTATUS currentTurtlePenStatus)
{
    RECT rectangleEncompassingControlArea, rectangleEncompassingButtonForChangingPenStatus;
    UINT widthOfControlArea, heightOfControlArea, widthOfButton, heightOfButton;
    UINT numberOfCharactersWithinButton;

    rectangleEncompassingControlArea = GetRectangleEncompassingControlArea(handleToWindow);
    widthOfControlArea = rectangleEncompassingControlArea.right - rectangleEncompassingControlArea.left;
    heightOfControlArea = rectangleEncompassingControlArea.bottom - rectangleEncompassingControlArea.top;

    if (currentTurtlePenStatus == PEN_UP) {
        numberOfCharactersWithinButton = _tcslen(TEXT_WITHIN_BUTTON_FOR_SETTING_PEN_DOWN);
    }
    else {
        numberOfCharactersWithinButton = _tcslen(TEXT_WITHIN_BUTTON_FOR_SETTING_PEN_UP);
    }

    widthOfButton = GetWidthOfModerateSizedButton(handleToWindow, numberOfCharactersWithinButton);
    heightOfButton = GetHeightOfModerateSizedButton(handleToWindow);

    rectangleEncompassingButtonForChangingPenStatus.left = 3 * widthOfControlArea / 4;
    rectangleEncompassingButtonForChangingPenStatus.right = rectangleEncompassingButtonForChangingPenStatus.left +
        widthOfButton;
    rectangleEncompassingButtonForChangingPenStatus.top = rectangleEncompassingControlArea.top +
        (heightOfControlArea / 12);
    rectangleEncompassingButtonForChangingPenStatus.bottom = rectangleEncompassingButtonForChangingPenStatus.top +
        heightOfButton;
    return rectangleEncompassingButtonForChangingPenStatus;
}


RECT GetRectangleEncompassingControlArea(HWND handleToWindow)
{
    RECT rectangleEncompassingControlArea;

    rectangleEncompassingControlArea.left = 0;
    rectangleEncompassingControlArea.top = GetTopOffsetOfCanvas(handleToWindow) + GetHeightOfCanvas(handleToWindow) +
        GetBottomOffsetOfCanvas(handleToWindow);
    rectangleEncompassingControlArea.right = GetWidthOfClientArea(handleToWindow);
    rectangleEncompassingControlArea.bottom = GetHeightOfClientArea(handleToWindow);
    return rectangleEncompassingControlArea;
}


RECT GetRectangleEncompassingPoints(CONST POINT points[], UINT numberOfPoints)
{
    RECT rectangleEncompassingPoints;
    int i;

    rectangleEncompassingPoints.left = points[0].x;
    rectangleEncompassingPoints.right = points[0].x;
    rectangleEncompassingPoints.top = points[0].y;
    rectangleEncompassingPoints.bottom = points[0].y;

    for (i = 1; i < numberOfPoints; i++) {
        if (points[i].x < rectangleEncompassingPoints.left) {
            rectangleEncompassingPoints.left = points[i].x;
        }
        else if (points[i].x > rectangleEncompassingPoints.right) {
            rectangleEncompassingPoints.right = points[i].x;
        }

        if (points[i].y < rectangleEncompassingPoints.top) {
            rectangleEncompassingPoints.top = points[i].y;
        }
        else if (points[i].y > rectangleEncompassingPoints.bottom) {
            rectangleEncompassingPoints.bottom = points[i].y;
        }
    }

    rectangleEncompassingPoints.right += 1;
    rectangleEncompassingPoints.bottom += 1;

    return rectangleEncompassingPoints;
}


RECT GetRectangleEncompassingRectangles(UINT numberOfRectangles, RECT firstRectangle, ...)
{
    va_list argumentList;
    RECT nextRectangle, rectangleEncompassingRectangles;
    int count;

    va_start(argumentList, firstRectangle);
    rectangleEncompassingRectangles = firstRectangle;

    for (count = 2; count <= numberOfRectangles; count++) {
        nextRectangle = va_arg(argumentList, RECT);

        if (nextRectangle.left < rectangleEncompassingRectangles.left) {
            rectangleEncompassingRectangles.left = nextRectangle.left;
        }

        if (nextRectangle.right > rectangleEncompassingRectangles.right) {
            rectangleEncompassingRectangles.right = nextRectangle.right;
        }

        if (nextRectangle.top < rectangleEncompassingRectangles.top) {
            rectangleEncompassingRectangles.top = nextRectangle.top;
        }

        if (nextRectangle.bottom > rectangleEncompassingRectangles.bottom) {
            rectangleEncompassingRectangles.bottom = nextRectangle.bottom;
        }
    }

    va_end(argumentList);

    return rectangleEncompassingRectangles;
}


LINE GetLineAlongTopOfCanvas(RECT rectangleEncompassingCanvas, RECT rectangleEncompassingAreaAroundCanvas)
{
    LINE lineAlongTopOfCanvas;

    lineAlongTopOfCanvas.start.x = rectangleEncompassingAreaAroundCanvas.left;
    lineAlongTopOfCanvas.start.y = rectangleEncompassingCanvas.top;
    lineAlongTopOfCanvas.end.x = rectangleEncompassingAreaAroundCanvas.right;
    lineAlongTopOfCanvas.end.y = rectangleEncompassingCanvas.top;
    return lineAlongTopOfCanvas;
}


LINE GetLineAlongBottomOfCanvas(RECT rectangleEncompassingCanvas, RECT rectangleEncompassingAreaAroundCanvas)
{
    LINE lineAlongBottomOfCanvas;

    lineAlongBottomOfCanvas.start.x = rectangleEncompassingAreaAroundCanvas.left;
    lineAlongBottomOfCanvas.start.y = rectangleEncompassingCanvas.bottom;
    lineAlongBottomOfCanvas.end.x = rectangleEncompassingAreaAroundCanvas.right;
    lineAlongBottomOfCanvas.end.y = rectangleEncompassingCanvas.bottom;
    return lineAlongBottomOfCanvas;
}


LINE GetLineAlongLeftOfCanvas(RECT rectangleEncompassingCanvas, RECT rectangleEncompassingAreaAroundCanvas)
{
    LINE lineAlongLeftOfCanvas;

    lineAlongLeftOfCanvas.start.x = rectangleEncompassingCanvas.left;
    lineAlongLeftOfCanvas.start.y = rectangleEncompassingAreaAroundCanvas.top;
    lineAlongLeftOfCanvas.end.x = rectangleEncompassingCanvas.left;
    lineAlongLeftOfCanvas.end.y = rectangleEncompassingAreaAroundCanvas.bottom;
    return lineAlongLeftOfCanvas;
}


LINE GetLineAlongRightOfCanvas(RECT rectangleEncompassingCanvas, RECT rectangleEncompassingAreaAroundCanvas)
{
    LINE lineAlongRightOfCanvas;

    lineAlongRightOfCanvas.start.x = rectangleEncompassingCanvas.right;
    lineAlongRightOfCanvas.start.y = rectangleEncompassingAreaAroundCanvas.top;
    lineAlongRightOfCanvas.end.x = rectangleEncompassingCanvas.right;
    lineAlongRightOfCanvas.end.y = rectangleEncompassingAreaAroundCanvas.bottom;
    return lineAlongRightOfCanvas;
}


CARTESIANPOINT GetLogicalCenterOfShellOfTurtle(double directionWhereTurtleIsFacing)
{
    CARTESIANPOINT logicalCenterOfShellOfTurtle = {-90, 90};
    RotatePoints(&logicalCenterOfShellOfTurtle, 1, directionWhereTurtleIsFacing);
    return logicalCenterOfShellOfTurtle;
}


CARTESIANPOINT *GetLogicalVerticesOfHeadOfTurtle(double directionWhereTurtleIsFacing)
{
    static CARTESIANPOINT workingLogicalVertices[NUMBER_OF_VERTICES_OF_HEAD_OF_TURTLE];
    CONST CARTESIANPOINT baseLogicalVertices[NUMBER_OF_VERTICES_OF_HEAD_OF_TURTLE] = {
        {-55, 110}, {-30, 110}, {-20, 100}, {-20, 80}, {-30, 70}, {-55, 70}, {-60, 75}, {-60, 105}};

    memcpy(workingLogicalVertices, baseLogicalVertices, sizeof(baseLogicalVertices));
    RotatePoints(workingLogicalVertices, NUMBER_OF_VERTICES_OF_HEAD_OF_TURTLE, directionWhereTurtleIsFacing);
    return workingLogicalVertices;
}


CARTESIANPOINT *GetLogicalVerticesOfPenOfTurtle(double directionWhereTurtleIsFacing, TURTLEPENSTATUS turtlePenStatus)
{
    static CARTESIANPOINT workingLogicalVertices[NUMBER_OF_VERTICES_OF_PEN_OF_TURTLE];
    CONST CARTESIANPOINT baseLogicalVerticesForPenThatIsUp[NUMBER_OF_VERTICES_OF_PEN_OF_TURTLE] = {
        {-80, 80}, {-60, 70}, {0, 10}, {-10, 0}, {-70, 60}};
    CONST CARTESIANPOINT baseLogicalVerticesForPenThatIsDown[NUMBER_OF_VERTICES_OF_PEN_OF_TURTLE] = {
        {-85, 75}, {-75, 85}, {-15, 25}, {0, 0}, {-25, 15}};

    if (turtlePenStatus == PEN_UP) {
        memcpy(workingLogicalVertices, baseLogicalVerticesForPenThatIsUp, sizeof(workingLogicalVertices));
    }
    else {
        memcpy(workingLogicalVertices, baseLogicalVerticesForPenThatIsDown, sizeof(workingLogicalVertices));
    }

    RotatePoints(workingLogicalVertices, NUMBER_OF_VERTICES_OF_PEN_OF_TURTLE, directionWhereTurtleIsFacing);
    return workingLogicalVertices;
}


CARTESIANPOINT *GetLogicalVerticesOfRightForeLimbOfTurtle(double directionWhereTurtleIsFacing)
{
    static CARTESIANPOINT workingLogicalVertices[NUMBER_OF_VERTICES_OF_RIGHT_FORE_LIMB_OF_TURTLE];
    CONST CARTESIANPOINT baseLogicalVertices[NUMBER_OF_VERTICES_OF_RIGHT_FORE_LIMB_OF_TURTLE] = {
        {-85, 50}, {-75, 55}, {-75, 45}, {-30, 45}, {-30, 42}, {-35, 42}, {-30, 42}, {-30, 38}, 
        {-35, 38}, {-30, 38}, {-30, 35}, {-80, 30}, {-85, 35}};

    memcpy(workingLogicalVertices, baseLogicalVertices, sizeof(baseLogicalVertices));
    RotatePoints(workingLogicalVertices, NUMBER_OF_VERTICES_OF_RIGHT_FORE_LIMB_OF_TURTLE, directionWhereTurtleIsFacing);
    return workingLogicalVertices;
}


CARTESIANPOINT *GetLogicalVerticesOfLeftForeLimbOfTurtle(double directionWhereTurtleIsFacing)
{
    static CARTESIANPOINT workingLogicalVertices[NUMBER_OF_VERTICES_OF_LEFT_FORE_LIMB_OF_TURTLE];
    CONST CARTESIANPOINT baseLogicalVertices[NUMBER_OF_VERTICES_OF_LEFT_FORE_LIMB_OF_TURTLE] = {
        {-85, 130}, {-75, 125}, {-75, 135}, {-30, 135}, {-30, 138}, {-35, 138}, {-30, 138}, {-30, 142},
        {-35, 142}, {-30, 142}, {-30, 145}, {-80, 150}, {-85, 145}};

    memcpy(workingLogicalVertices, baseLogicalVertices, sizeof(baseLogicalVertices));
    RotatePoints(workingLogicalVertices, NUMBER_OF_VERTICES_OF_LEFT_FORE_LIMB_OF_TURTLE, directionWhereTurtleIsFacing);
    return workingLogicalVertices;
}


CARTESIANPOINT *GetLogicalVerticesOfRightHindLimbOfTurtle(double directionWhereTurtleIsFacing)
{
    static CARTESIANPOINT workingLogicalVertices[NUMBER_OF_VERTICES_OF_RIGHT_HIND_LIMB_OF_TURTLE];
    CONST CARTESIANPOINT baseLogicalVertices[NUMBER_OF_VERTICES_OF_RIGHT_HIND_LIMB_OF_TURTLE] = {
        {-95, 50}, {-105, 55}, {-105, 45}, {-150, 45}, {-150, 42}, {-145, 42}, {-150, 42}, {-150, 38},
        {-145, 38}, {-150, 38}, {-150, 35}, {-100, 30}, {-95, 35}};

    memcpy(workingLogicalVertices, baseLogicalVertices, sizeof(baseLogicalVertices));
    RotatePoints(workingLogicalVertices, NUMBER_OF_VERTICES_OF_RIGHT_HIND_LIMB_OF_TURTLE, directionWhereTurtleIsFacing);
    return workingLogicalVertices;
}


CARTESIANPOINT *GetLogicalVerticesOfLeftHindLimbOfTurtle(double directionWhereTurtleIsFacing)
{
    static CARTESIANPOINT workingLogicalVertices[NUMBER_OF_VERTICES_OF_LEFT_HIND_LIMB_OF_TURTLE];
    CONST CARTESIANPOINT baseLogicalVertices[NUMBER_OF_VERTICES_OF_LEFT_HIND_LIMB_OF_TURTLE] = {
        {-95, 130}, {-105, 125}, {-105, 135}, {-150, 135}, {-150, 138}, {-145, 138}, {-150, 138},
        {-150, 142}, {-145, 142}, {-150, 142}, {-150, 145}, {-100, 150}, {-95, 145}};

    memcpy(workingLogicalVertices, baseLogicalVertices, sizeof(baseLogicalVertices));
    RotatePoints(workingLogicalVertices, NUMBER_OF_VERTICES_OF_LEFT_HIND_LIMB_OF_TURTLE, directionWhereTurtleIsFacing);
    return workingLogicalVertices;
}


UINT GetLeftOffsetOfCanvas(HWND handleToWindow)
{
    UINT widthOfClientArea, widthOfCanvas, minimumPossibleOffsetOfCanvas;

    widthOfClientArea = GetWidthOfClientArea(handleToWindow);
    widthOfCanvas = GetWidthOfCanvas(handleToWindow);
    minimumPossibleOffsetOfCanvas = GetMinimumPossibleOffsetOfCanvas(handleToWindow);

    if (((widthOfClientArea - widthOfCanvas) / 2) > minimumPossibleOffsetOfCanvas) {
        return (widthOfClientArea - widthOfCanvas) / 2;
    }
    else {
        return minimumPossibleOffsetOfCanvas;
    }
}


UINT GetTopOffsetOfCanvas(HWND handleToWindow)
{
    return GetMinimumPossibleOffsetOfCanvas(handleToWindow);
}


UINT GetBottomOffsetOfCanvas(HWND handleToWindow)
{
    return GetMinimumPossibleOffsetOfCanvas(handleToWindow);
}


UINT GetWidthOfCanvas(HWND handleToWindow)
{
    UINT widthOfClientArea, heightOfClientArea, minimumPossibleOffsetOfCanvas, minimumPossibleHeightOfControlArea;
    INT maximumPossibleWidthOfCanvas, maximumPossibleHeightOfCanvas;

    widthOfClientArea = GetWidthOfClientArea(handleToWindow);
    heightOfClientArea = GetHeightOfClientArea(handleToWindow);
    minimumPossibleOffsetOfCanvas = GetMinimumPossibleOffsetOfCanvas(handleToWindow);
    minimumPossibleHeightOfControlArea = GetMinimumPossibleHeightOfControlArea(handleToWindow);

    maximumPossibleWidthOfCanvas = widthOfClientArea - (2 * minimumPossibleOffsetOfCanvas);
    if (maximumPossibleWidthOfCanvas < 0) {
        maximumPossibleWidthOfCanvas = 0;
    }

    maximumPossibleHeightOfCanvas = heightOfClientArea - minimumPossibleHeightOfControlArea - (2 *
        minimumPossibleOffsetOfCanvas);
    if (maximumPossibleHeightOfCanvas < 0) {
        maximumPossibleHeightOfCanvas = 0;
    }

    if (maximumPossibleWidthOfCanvas < maximumPossibleHeightOfCanvas) {
        return maximumPossibleWidthOfCanvas;
    }
    else {
        return maximumPossibleHeightOfCanvas;
    }
}


UINT GetMinimumPossibleOffsetOfCanvas(HWND handleToWindow)
{
    UINT widthOfClientArea, heightOfClientArea;

    widthOfClientArea = GetWidthOfClientArea(handleToWindow);
    heightOfClientArea = GetHeightOfClientArea(handleToWindow);

    if (widthOfClientArea < heightOfClientArea) {
        return widthOfClientArea / 10;
    }
    else {
        return heightOfClientArea / 10;
    }
}


UINT GetHeightOfCanvas(HWND handleToWindow)
{
    return GetWidthOfCanvas(handleToWindow); // Height of canvas is equal to width of canvas
}


UINT GetWidthOfClientArea(HWND handleToWindow)
{
    RECT rectangleEncompassingClientArea;
    GetClientRect(handleToWindow, &rectangleEncompassingClientArea);
    return rectangleEncompassingClientArea.right - rectangleEncompassingClientArea.left;
}


UINT GetHeightOfClientArea(HWND handleToWindow)
{
    RECT rectangleEncompassingClientArea;
    GetClientRect(handleToWindow, &rectangleEncompassingClientArea);
    return rectangleEncompassingClientArea.bottom - rectangleEncompassingClientArea.top;
}


UINT GetMinimumPossibleHeightOfControlArea(HWND handleToWindow)
{
    return 6 * GetMaximumCharacterHeight(handleToWindow);
}


UINT GetWidthOfModerateSizedButton(HWND handleToWindow, WORD numberOfCharactersWithinButton)
{
    return (4 + numberOfCharactersWithinButton) * GetAverageCharacterWidth(handleToWindow);
}


UINT GetHeightOfModerateSizedButton(HWND handleToWindow)
{
    return 2 * GetMaximumCharacterHeight(handleToWindow);
}


UINT GetMaximumCharacterHeight(HWND handleToWindow)
{
    static UINT maximumCharacterHeight = 0;

    if (maximumCharacterHeight == 0) {
        TEXTMETRIC textMetrics;
        HDC handleToInformationContext = CreateIC(TEXT("DISPLAY"), NULL, NULL, NULL);
        GetTextMetrics(handleToInformationContext, &textMetrics);
        maximumCharacterHeight = textMetrics.tmHeight + textMetrics.tmExternalLeading;
        DeleteDC(handleToInformationContext);
    }

    return maximumCharacterHeight;
}


UINT GetAverageCharacterWidth(HWND handleToWindow)
{
    static UINT averageCharacterWidth = 0;

    if (averageCharacterWidth == 0) {
        TEXTMETRIC textMetrics;
        HDC handleToInformationContext = CreateIC(TEXT("DISPLAY"), NULL, NULL, NULL);
        GetTextMetrics(handleToInformationContext, &textMetrics);
        averageCharacterWidth = textMetrics.tmAveCharWidth;
        DeleteDC(handleToInformationContext);
    }

    return averageCharacterWidth;
}