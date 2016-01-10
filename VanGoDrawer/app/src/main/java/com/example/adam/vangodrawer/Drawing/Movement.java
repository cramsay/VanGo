package com.example.adam.vangodrawer.Drawing;

/**
 * Created by Adam on 07/01/2016.
 */
public class Movement {
    private int x, y;
    private boolean pendown;

    public Movement(int x, int y, boolean pendown){
        this.x = x;
        this.y = y;
        this.pendown = pendown;
    }

    @Override
    public String toString(){
        if(pendown) {
            return x + ", " + y + ", 1";
        }
        return x + ", " + y + ", 0";
    }
}
