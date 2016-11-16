package ru.tensor.snake.game;

import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.CornerPathEffect;
import android.graphics.LightingColorFilter;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Path;
import android.util.DisplayMetrics;
import android.util.TypedValue;

import java.util.ArrayList;

import ru.tensor.generated.Field;
import ru.tensor.generated.SnakeModel;
import ru.tensor.generated.VectorModel;
import ru.tensor.generated.Walls;

final class GameDrawer {

    DisplayMetrics displayMetrics;
    Resources resources;

    private final Paint whitePaint;
    private final Paint blackPaint;
    private final Paint enemySnakePaint;
    private final Paint mySnakePaint;
    private final Paint wallsPaint;

    private final Bitmap backgroundImage;

    private VectorModel cameraPos = new VectorModel(0, 0);
    private Matrix matrix = new Matrix();

    public GameDrawer(Resources res) {
        resources = res;
        displayMetrics = resources.getDisplayMetrics();

        whitePaint = new Paint() {
            {
                setStyle(Paint.Style.FILL);
                setAntiAlias(true);
                setColor(Color.WHITE);
            }
        };

        blackPaint = new Paint() {
            {
                setStyle(Paint.Style.FILL);
                setAntiAlias(true);
                setColor(Color.BLACK);
            }
        };

        enemySnakePaint = new Paint() {
            {
                setStyle(Paint.Style.STROKE);
                setStrokeCap(Paint.Cap.ROUND);
                setStrokeWidth(mmToPixels(4));
                setAntiAlias(true);
                setColor(Color.RED);
                setStrokeCap(Paint.Cap.ROUND);
                setStrokeJoin(Paint.Join.ROUND);
                setPathEffect(new CornerPathEffect(mmToPixels(10)));
            }
        };

        mySnakePaint = new Paint(enemySnakePaint) {
            {
                setColor(Color.BLUE);
            }
        };

        wallsPaint = new Paint() {
            {
                setColor(Color.YELLOW);
                setStyle(Style.STROKE);
                setPathEffect(new CornerPathEffect(mmToPixels(5)));
                setStrokeWidth(mmToPixels(1.5f));
                setAlpha(128);
            }
        };

        backgroundImage = BitmapFactory.decodeResource(resources, R.drawable.background);
    }

    public void draw(Canvas canvas, Field fld) {
        SnakeModel my_snake = fld.getMySnake();

        if (my_snake != null)
            cameraPos = my_snake.getPoints().get(0);
        drawBackground(canvas, cameraPos);

        matrix.setTranslate(canvas.getWidth() / 2 - mmToPixels(cameraPos.getX()),
                canvas.getHeight() / 2 - mmToPixels(cameraPos.getY()));
        canvas.setMatrix(matrix);

        // рисуем стены
        drawWalls(canvas, fld.getWalls());

        // рисуем змей
        if (my_snake != null)
            drawSnake(canvas, mySnakePaint, my_snake);

        ArrayList<SnakeModel> snakes = fld.getSnakes();
        for (SnakeModel snake : snakes)
            drawSnake(canvas, enemySnakePaint, snake);

        // рисуем бонусы
        ArrayList<VectorModel> bonuses = fld.getBonuses();
        for (VectorModel b : bonuses)
            drawBonus(canvas, b);
    }

    private float mmToPixels(float sizeInMM) {
        return TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_MM, sizeInMM, displayMetrics);
    }

    private void drawEye(Canvas canvas, VectorModel pos) {
        canvas.drawCircle(mmToPixels(pos.getX()), mmToPixels(pos.getY()), mmToPixels(0.7f), whitePaint);
        canvas.drawCircle(mmToPixels(pos.getX()), mmToPixels(pos.getY()), mmToPixels(0.3f), blackPaint);
    }

    private void drawSnake(Canvas canvas, Paint paint, SnakeModel snake) {
        ArrayList<VectorModel> points = snake.getPoints();
        if (points.isEmpty())
            return;

        // рисуем тело
        Path path = new Path();
        path.moveTo(mmToPixels(points.get(0).getX()), mmToPixels(points.get(0).getY()));
        for (final VectorModel p : points)
            path.lineTo(mmToPixels(p.getX()), mmToPixels(p.getY()));

        for (int i = 0; i < 3; ++i) {
            paint.setStrokeWidth(mmToPixels(3f - 0.4f * i));
            paint.setColorFilter(new LightingColorFilter(Color.rgb(128 + i * 40, 128 + i * 30, 128 + i * 30), 0));
            canvas.drawPath(path, paint);
        }

        // рисуем глаза
        drawEye(canvas, snake.getLeftEye());
        drawEye(canvas, snake.getRightEye());
    }

    private void drawBonus(Canvas canvas, VectorModel position) {
        canvas.drawCircle(mmToPixels(position.getX()), mmToPixels(position.getY()),
                mmToPixels(5), whitePaint);
    }

    private void drawBackground(Canvas canvas, VectorModel startPoint) {
        float w = backgroundImage.getWidth();
        float h = backgroundImage.getHeight();
        for (float x = -mmToPixels(startPoint.getX()) % w - w; x < canvas.getWidth(); x += w)
            for (float y = -mmToPixels(startPoint.getY()) % h - h; y < canvas.getHeight(); y += h)
                canvas.drawBitmap(backgroundImage, x, y, null);
    }

    private void drawWalls(Canvas canvas, Walls walls) {
        Path p = new Path();
        p.moveTo(mmToPixels(walls.getLeftX()), mmToPixels(walls.getTopY()));
        p.lineTo(mmToPixels(walls.getRightX()), mmToPixels(walls.getTopY()));
        p.lineTo(mmToPixels(walls.getRightX()), mmToPixels(walls.getBottomY()));
        p.lineTo(mmToPixels(walls.getLeftX()), mmToPixels(walls.getBottomY()));
        p.close();
        canvas.drawPath(p, wallsPaint);
    }

}
