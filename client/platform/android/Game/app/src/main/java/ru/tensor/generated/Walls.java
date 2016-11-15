// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from walls.djinni

package ru.tensor.generated;

public final class Walls {


    /*package*/ final int mLeftX;

    /*package*/ final int mTopY;

    /*package*/ final int mRightX;

    /*package*/ final int mBottomY;

    public Walls(
            int leftX,
            int topY,
            int rightX,
            int bottomY) {
        this.mLeftX = leftX;
        this.mTopY = topY;
        this.mRightX = rightX;
        this.mBottomY = bottomY;
    }

    public int getLeftX() {
        return mLeftX;
    }

    public int getTopY() {
        return mTopY;
    }

    public int getRightX() {
        return mRightX;
    }

    public int getBottomY() {
        return mBottomY;
    }

    @Override
    public String toString() {
        return "Walls{" +
                "mLeftX=" + mLeftX +
                "," + "mTopY=" + mTopY +
                "," + "mRightX=" + mRightX +
                "," + "mBottomY=" + mBottomY +
        "}";
    }

}
