package com.example.adam.vangodrawer;

import java.util.List;

/**
 * Created by Adam on 07/01/2016.
 */
public class DrawingReader {

    private LineManager lineManager;
    private int currentLine, currentSeg, x, y;

    public DrawingReader(LineManager lineManager){
        this.lineManager = lineManager;
        currentLine = 0;
        currentSeg = 0;
        x = 0;
        y = 0;
    }

    public String nextMove(){
        List<FullLine> lines = lineManager.getLines();
        //If there are still lines to send
        if(lines.size()>currentLine){
            FullLine thisLine = lines.get(currentLine);
            if(thisLine.isDot()){
                //If the line is a dot...
                if(x == thisLine.getStartX()&&y == thisLine.getStartY()){
                    //... And we're in the right position, draw the dot and move on to the next line
                    currentLine++;
                    currentSeg = 0;
                    return new Movement(0,0,true).toString();
                } else {
                    //If not, move to the correct position and leave drawing the dot to the next call
                    x = (int) thisLine.getStartX();
                    y = (int) thisLine.getStartY();
                    return new Movement((int)(thisLine.getStartX()-x), (int)(thisLine.getStartY()-y), false).toString();
                }
            } else {
                //It's not a dot
                //If the current segment exists
                if(thisLine.getLine().size()>currentSeg){
                    LineSegment thisSeg = thisLine.getLine().get(currentSeg);
                    if(thisSeg.getStartX()==x && thisSeg.getStartY()==y){
                        //If we're in the right position to start
                        x = (int)thisSeg.getEndX();
                        y = (int)thisSeg.getEndY();
                        currentSeg++;
                        return new Movement((int)(thisSeg.getEndX()-thisSeg.getStartX()),
                                (int)(thisSeg.getEndY()- thisSeg.getStartY()), true).toString();
                    } else {
                        //We need to get in position to start drawing the line
                        Movement thisMov = new Movement (((int)thisSeg.getStartX()-x), ((int) thisSeg.getStartY()-y),false);
                        x = (int) thisSeg.getStartX();
                        y = (int) thisSeg.getStartY();
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
