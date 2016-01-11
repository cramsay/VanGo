package com.example.adam.vangodrawer.Drawing;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by Adam on 03/01/2016.
 */
public class LineManager {

    private FullLine currentLine;
    private List<FullLine> lines;
    private int xSize, ySize, paperX, paperY;

    public LineManager(int width, int height, int paperWidth, int paperHeight){
        currentLine = null;
        lines = new ArrayList<FullLine>();
        this.xSize = width;
        this.ySize = height;
        this.paperX = paperWidth;
        this.paperY = paperHeight;
    }

    public void newLine(float startX, float startY){
        int newX = (int) (startX/xSize) * paperX;
        int newY = (int) (startY/ySize) * paperY;
        currentLine = new FullLine(newX, newY);
        lines.add(currentLine);
    }

    public void addSegment(float x, float y){
        int newX = (int) (x/xSize) * paperX;
        int newY = (int) (y/ySize) * paperY;
        currentLine.addSegmentTo(newX, newY);
    }

    public List<FullLine> getLines(){
        return new ArrayList<FullLine>(lines);
    }

}
