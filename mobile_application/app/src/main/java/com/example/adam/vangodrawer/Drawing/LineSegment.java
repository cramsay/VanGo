package com.example.adam.vangodrawer.Drawing;

/**
 * Created by Adam on 03/01/2016.
 */
public class LineSegment {

    private int startX, startY, endX, endY;

    public LineSegment(int startX, int startY, int endX, int endY){
        this.startX = startX;
        this.startY = startY;
        this.endX = endX;
        this.endY = endY;
    }

    public int getStartX() {
        return startX;
    }

    public int getStartY() {
        return startY;
    }

    public int getEndX() {
        return endX;
    }

    public int getEndY() {
        return endY;
    }
}
