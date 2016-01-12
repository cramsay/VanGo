package com.example.adam.vangodrawer.Drawing;

import android.util.Log;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by Adam on 03/01/2016.
 */
public class FullLine {

    final String TAG = "FullLine";
    private List<LineSegment> line;
    private int startX, startY, currentX, currentY;

    public FullLine(int startX, int startY){
        line = new ArrayList<LineSegment>();
        this.startX = startX;
        this.startY = startY;
        currentX = startX;
        currentY = startY;
    }

    public void addSegmentTo(int x, int y){
        line.add(new LineSegment(currentX, currentY, x, y));
        currentX = x;
        currentY = y;
    }

    public List<LineSegment> getLine(){
        return new ArrayList<LineSegment>(line);
    }

    public int getStartX(){
        return startX;
    }

    public int getStartY(){
        return startY;
    }

    public boolean isDot(){
        return line.isEmpty();
    }
}
