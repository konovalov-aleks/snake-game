// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from snake.djinni

package ru.tensor.generated;

import java.util.ArrayList;

public final class SnakeModel {


    /*package*/ final ArrayList<VectorModel> mPoints;

    /*package*/ final VectorModel mLeftEye;

    /*package*/ final VectorModel mRightEye;

    public SnakeModel(
            ArrayList<VectorModel> points,
            VectorModel leftEye,
            VectorModel rightEye) {
        this.mPoints = points;
        this.mLeftEye = leftEye;
        this.mRightEye = rightEye;
    }

    public ArrayList<VectorModel> getPoints() {
        return mPoints;
    }

    public VectorModel getLeftEye() {
        return mLeftEye;
    }

    public VectorModel getRightEye() {
        return mRightEye;
    }

    @Override
    public String toString() {
        return "SnakeModel{" +
                "mPoints=" + mPoints +
                "," + "mLeftEye=" + mLeftEye +
                "," + "mRightEye=" + mRightEye +
        "}";
    }

}
