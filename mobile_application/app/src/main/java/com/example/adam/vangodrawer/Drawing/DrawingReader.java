package com.example.adam.vangodrawer.Drawing;

import android.util.Log;

import java.util.List;

/**
 * Created by Adam on 07/01/2016.
 */
public class DrawingReader {

    private final String TAG = "DrawingReader";
    private LineManager lineManager;
    private int currentLine, currentSeg;
    private boolean inPosition;

    public DrawingReader(LineManager lineManager){
        this.lineManager = lineManager;
        currentLine = 0;
        currentSeg = 0;
        inPosition = false;
    }

    public String nextMove(){
        List<FullLine> lines = lineManager.getLines();
        //If there are still lines to send
        if(lines.size()>currentLine){
            FullLine thisLine = lines.get(currentLine);
            if(thisLine.isDot()){
                //If the line is a dot...
                if(inPosition){
                    //... And we're in the right position, draw the dot and move on to the next line
                    currentLine++;
                    currentSeg = 0;
                    inPosition = false;
                    return new Movement(thisLine.getStartX(), 0-thisLine.getStartY(),true).toString();
                } else {
                    //If not, move to the correct position and leave drawing the dot to the next call
                    String s = new Movement(thisLine.getStartX(), 0-thisLine.getStartY(), false).toString();
                    inPosition = true;
                    return s;
                }
            } else {
                //It's not a dot
                //If the current segment exists
                if(thisLine.getLine().size()>currentSeg){
                    LineSegment thisSeg = thisLine.getLine().get(currentSeg);
                    if(inPosition){
                        //If we're in the right position to start
                        if(thisLine.getLine().size()==++currentSeg){
                            currentSeg = 0;
                            currentLine++;
                            inPosition = false;
                        }
                        return new Movement(thisSeg.getEndX(),
                                0-thisSeg.getEndY(), true).toString();
                    } else {
                        //We need to get in position to start drawing the line
                        Movement thisMov = new Movement (thisSeg.getStartX(), 0-thisSeg.getStartY(),false);
                        inPosition = true;
                        return thisMov.toString();

                    }
                } else {
                    //If the currentSeq doesn't exist move on a line and call the method again
                    currentLine++;
                    currentSeg = 0;
                    return nextMove();
                }
            }
        }
        //Returns null when there's nothing left
        return null;
    }
}
