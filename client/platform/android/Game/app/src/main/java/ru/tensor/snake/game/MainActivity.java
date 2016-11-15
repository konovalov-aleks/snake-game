package ru.tensor.snake.game;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.CornerPathEffect;
import android.graphics.LightingColorFilter;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Path;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import java.util.ArrayList;

import ru.tensor.generated.VectorModel;
import ru.tensor.sbis.core.Initializer;
import ru.tensor.snake.game.R;
import ru.tensor.generated.Field;
import ru.tensor.generated.Game;
import ru.tensor.generated.Point;
import ru.tensor.generated.SnakeModel;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Initializer.init(this, "snake-game");
        setContentView(new DrawView(this));
    }

    class DrawView extends SurfaceView implements SurfaceHolder.Callback, View.OnTouchListener {

        private DrawThread drawThread;
        private boolean mTouchPressed = false;
        private int mTouchX, mTouchY;

        public DrawView(Context context) {
            super(context);
            setOnTouchListener(this);
            getHolder().addCallback(this);
        }

        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width,
                                   int height) {

        }

        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            drawThread = new DrawThread(getHolder());
            drawThread.setRunning(true);
            drawThread.start();
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            boolean retry = true;
            drawThread.setRunning(false);
            while (retry) {
                try {
                    drawThread.join();
                    retry = false;
                } catch (InterruptedException e) {
                }
            }
        }

        @Override
        public boolean onTouch(View view, MotionEvent motionEvent) {
            switch(motionEvent.getAction()) {
                case MotionEvent.ACTION_MOVE:
                case MotionEvent.ACTION_DOWN:
                    mTouchPressed = true;
                    mTouchX = (int)motionEvent.getX();
                    mTouchY = (int)motionEvent.getY();
                    int dx = mTouchX - view.getWidth() / 2;
                    int dy = mTouchY - view.getHeight() / 2;
                    Game.instance().setDirection(dx, dy);
                    return true;
                case MotionEvent.ACTION_UP:
                    mTouchPressed = false;
                    break;
            }
            return false;
        }

        class DrawThread extends Thread {

            private boolean running = false;
            private SurfaceHolder surfaceHolder;

            public DrawThread(SurfaceHolder surfaceHolder) {
                this.surfaceHolder = surfaceHolder;
            }

            public void setRunning(boolean running) {
                this.running = running;
            }

            final Paint WHITE_PAINT = new Paint() {
                {
                    setStyle(Paint.Style.FILL);
                    setAntiAlias(true);
                    setColor(Color.WHITE);
                }
            };

            final Paint BLACK_PAINT = new Paint() {
                {
                    setStyle(Paint.Style.FILL);
                    setAntiAlias(true);
                    setColor(Color.BLACK);
                }
            };

            private void drawEye(Canvas canvas, VectorModel pos) {
                canvas.drawCircle(pos.getX(), pos.getY(), 4, WHITE_PAINT);
                canvas.drawCircle(pos.getX(), pos.getY(), 2, BLACK_PAINT);
            }

            private void drawSnake(Canvas canvas, Paint paint, SnakeModel snake) {
                ArrayList<VectorModel> points = snake.getPoints();
                if(points.isEmpty())
                    return;

                // рисуем тело
                Path path = new Path();
                path.moveTo(points.get(0).getX(), points.get(0).getY());
                for(final VectorModel p : points)
                    path.lineTo(p.getX(), p.getY());

                for(int i = 0; i < 3; ++i)
                {
                    paint.setStrokeWidth(20 - 4 * i);
                    paint.setColorFilter(new LightingColorFilter(Color.rgb(128 + i * 40, 128 + i * 30, 128 + i * 30), 0));
                    canvas.drawPath(path, paint);
                }

                // рисуем глаза
                drawEye(canvas, snake.getLeftEye());
                drawEye(canvas, snake.getRightEye());
            }

            Bitmap backgroundImage = BitmapFactory.decodeResource(getResources(), R.drawable.background);

            void drawBackground(Canvas canvas, VectorModel startPoint) {
                float w = backgroundImage.getWidth();
                float h = backgroundImage.getHeight();
                for(float x = - startPoint.getX() % w - w; x < canvas.getWidth(); x += w)
                    for(float y = - startPoint.getY() % h - h; y < canvas.getHeight(); y += h)
                        canvas.drawBitmap(backgroundImage, x, y, null);
            }

            @Override
            public void run() {
                Canvas canvas;
                Game.instance().enter();;
                Paint paint = new Paint() {
                    {
                        setStyle(Paint.Style.STROKE);
                        setStrokeCap(Paint.Cap.ROUND);
                        setStrokeWidth(20.0f);
                        setAntiAlias(true);
                        setColor(Color.RED);
                        setStrokeCap(Paint.Cap.ROUND);
                        setStrokeJoin(Paint.Join.ROUND);
                        setPathEffect(new CornerPathEffect(100));
                    }
                };
                Paint cur_pl_paint = new Paint(paint);
                cur_pl_paint.setColor(Color.BLUE);
                Paint rps_paint = new Paint();
                double lastFrameTime = System.currentTimeMillis();
                Matrix matrix = new Matrix();

                while (running) {
                    double curTime = System.currentTimeMillis();
                    double rps = curTime != lastFrameTime ? 1000 / (curTime - lastFrameTime) : 1000;
                    lastFrameTime = curTime;
                    try {
                        sleep(10);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    Game.instance().run();
                    Field fld = Game.instance().getField();
                    canvas = null;
                    try {
                        canvas = surfaceHolder.lockCanvas(null);
                        if (canvas == null)
                            continue;

                        VectorModel mySnakeHead = fld.getMySnake().getPoints().get(0);

                        drawBackground(canvas, mySnakeHead);
                        canvas.drawText(String.valueOf(rps), 10, 10, rps_paint);

                        matrix.setTranslate(canvas.getWidth() / 2 - mySnakeHead.getX(),
                                canvas.getHeight() / 2 - mySnakeHead.getY());
                        canvas.setMatrix(matrix);

                        SnakeModel my_snake = fld.getMySnake();
                        if(my_snake != null)
                            drawSnake(canvas, cur_pl_paint, my_snake);
                        ArrayList<SnakeModel> snakes = fld.getSnakes();
                        for(SnakeModel snake : snakes)
                            drawSnake(canvas, paint, snake);
                    } finally {
                        if (canvas != null) {
                            surfaceHolder.unlockCanvasAndPost(canvas);
                        }
                    }
                }
            }
        }

    }
}
