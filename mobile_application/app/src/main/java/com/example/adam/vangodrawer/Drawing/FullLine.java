package com.example.adam.vangodrawer.Drawing;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by Adam on 03/01/2016.
 */
public class FullLine {

    private List<LineSegment> line;
    private float startX, startY, currentX, currentY;

    public FullLine(float startX, float startY){
        line = new ArrayList<LineSegment>();
        this.startX = startX;
        this.startY = startY;
        currentX = startX;
        currentY = startY;
    }

    public void addSegmentTo(float x, float y){
        line.add(new LineSegment(currentX, x, currentY, y));
        currentX = x;
        currentY = y;
    }

    public List<LineSegment> getLine(){
        return new ArrayList<LineSegment>(line);
    }

    public float getStartX(){
        return startX;
    }

    public float getStartY(){
        return startY;
    }

    public boolean isDot(){
        return !line.isEmpty();
    }
}
