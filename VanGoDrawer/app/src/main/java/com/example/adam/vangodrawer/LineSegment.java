package com.example.adam.vangodrawer;

/**
 * Created by Adam on 03/01/2016.
 */
public class LineSegment {

    private float startX, startY, endX, endY;

    public LineSegment(float startX, float startY, float endX, float endY){
        this.startX = startX;
        this.startY = startY;
        this.endX = endX;
        this.endY = endY;
    }

    public float getStartX() {
        return startX;
    }

    public float getStartY() {
        return startY;
    }

    public float getEndX() {
        return endX;
    }

    public float getEndY() {
        return endY;
    }
}
