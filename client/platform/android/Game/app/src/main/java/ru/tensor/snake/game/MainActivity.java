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
import android.util.TypedValue;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.RelativeLayout;

import java.util.ArrayList;
import java.util.Vector;

import ru.tensor.generated.Field;
import ru.tensor.generated.Game;
import ru.tensor.generated.SnakeModel;
import ru.tensor.generated.VectorModel;
import ru.tensor.generated.Walls;
import ru.tensor.sbis.core.Initializer;

public class MainActivity extends Activity {

    RelativeLayout mainMenu;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if(savedInstanceState == null)
            Initializer.init(this, "snake-game");

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        FrameLayout game = new FrameLayout(this);

        mainMenu = (RelativeLayout) View.inflate(this, R.layout.main_menu, null);
        Button startBtn = (Button) mainMenu.findViewById(R.id.start_button);
        startBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Game.instance().enter();
                mainMenu.setVisibility(View.INVISIBLE);
            }
        });

        game.addView(new DrawView(this));
        game.addView(mainMenu);
        setContentView(game);
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
            switch (motionEvent.getAction()) {
                case MotionEvent.ACTION_MOVE:
                case MotionEvent.ACTION_DOWN:
                    mTouchPressed = true;
                    mTouchX = (int) motionEvent.getX();
                    mTouchY = (int) motionEvent.getY();
                    int dx = mTouchX - view.getWidth() / 2;
                    int dy = mTouchY - view.getHeight() / 2;
                    try {
                        Game.instance().setDirection(dx, dy);
                    }
                    catch(Exception ex) {
                        ex.printStackTrace();
                    }
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

            private float mmToPixels(float sizeInMM) {
                return TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_MM, sizeInMM,
                        getResources().getDisplayMetrics());
            }

            private void drawEye(Canvas canvas, VectorModel pos) {
                canvas.drawCircle(mmToPixels(pos.getX()), mmToPixels(pos.getY()), mmToPixels(0.7f), WHITE_PAINT);
                canvas.drawCircle(mmToPixels(pos.getX()), mmToPixels(pos.getY()), mmToPixels(0.3f), BLACK_PAINT);
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
                        mmToPixels(5), WHITE_PAINT);
            }

            private Bitmap backgroundImage = BitmapFactory.decodeResource(getResources(), R.drawable.background);

            private void drawBackground(Canvas canvas, VectorModel startPoint) {
                float w = backgroundImage.getWidth();
                float h = backgroundImage.getHeight();
                for (float x = -mmToPixels(startPoint.getX()) % w - w; x < canvas.getWidth(); x += w)
                    for (float y = -mmToPixels(startPoint.getY()) % h - h; y < canvas.getHeight(); y += h)
                        canvas.drawBitmap(backgroundImage, x, y, null);
            }

            private Paint wallsPaint = new Paint() {
                {
                    setColor(Color.YELLOW);
                    setStyle(Style.STROKE);
                    setPathEffect(new CornerPathEffect(mmToPixels(5)));
                    setStrokeWidth(mmToPixels(1.5f));
                    setAlpha(128);
                }
            };

            private void drawWalls(Canvas canvas, Walls walls) {
                Path p = new Path();
                p.moveTo(mmToPixels(walls.getLeftX()), mmToPixels(walls.getTopY()));
                p.lineTo(mmToPixels(walls.getRightX()), mmToPixels(walls.getTopY()));
                p.lineTo(mmToPixels(walls.getRightX()), mmToPixels(walls.getBottomY()));
                p.lineTo(mmToPixels(walls.getLeftX()), mmToPixels(walls.getBottomY()));
                p.close();
                canvas.drawPath(p, wallsPaint);
            }

            @Override
            public void run() {
                Canvas canvas;
                Paint paint = new Paint() {
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
                Paint cur_pl_paint = new Paint(paint);
                cur_pl_paint.setColor(Color.BLUE);
                double lastFrameTime = System.currentTimeMillis();

                Matrix matrix = new Matrix();
                VectorModel cameraPos = new VectorModel(0, 0);

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

                        SnakeModel my_snake = fld.getMySnake();

                        if( my_snake != null )
                            cameraPos = my_snake.getPoints().get(0);
                        drawBackground(canvas, cameraPos);

                        matrix.setTranslate(canvas.getWidth() / 2 - mmToPixels(cameraPos.getX()),
                                canvas.getHeight() / 2 - mmToPixels(cameraPos.getY()));
                        canvas.setMatrix(matrix);

                        // рисуем стены
                        drawWalls(canvas, fld.getWalls());

                        // рисуем змей
                        if (my_snake != null)
                            drawSnake(canvas, cur_pl_paint, my_snake);
                        else
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    mainMenu.setVisibility(View.VISIBLE);
                                }
                            });

                        ArrayList<SnakeModel> snakes = fld.getSnakes();
                        for (SnakeModel snake : snakes)
                            drawSnake(canvas, paint, snake);

                        // рисуем бонусы
                        ArrayList<VectorModel> bonuses = fld.getBonuses();
                        for (VectorModel b : bonuses)
                            drawBonus(canvas, b);
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
