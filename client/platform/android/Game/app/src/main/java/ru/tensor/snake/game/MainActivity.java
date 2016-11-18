package ru.tensor.snake.game;

import android.app.Activity;
import android.content.Context;
import android.graphics.Canvas;
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

import ru.tensor.generated.Field;
import ru.tensor.generated.Game;
import ru.tensor.sbis.core.Initializer;

public class MainActivity extends Activity {

    private RelativeLayout mainMenu;
    private boolean gameOver = true;

    GameDrawer drawer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (savedInstanceState == null) {
            Initializer.init(this, "snake-game");
            drawer = new GameDrawer(getResources());
        }

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        FrameLayout game = new FrameLayout(this);

        mainMenu = (RelativeLayout) View.inflate(this, R.layout.main_menu, null);
        Button startBtn = (Button) mainMenu.findViewById(R.id.start_button);
        startBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Game.instance().enter();
                gameOver = false;
                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                mainMenu.setVisibility(View.INVISIBLE);
            }
        });

        game.addView(new DrawView(this));
        game.addView(mainMenu);
        setContentView(game);
    }

    class DrawView extends SurfaceView implements SurfaceHolder.Callback, View.OnTouchListener {

        private DrawThread drawThread;
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
            if (!gameOver)
                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_MOVE:
                    case MotionEvent.ACTION_DOWN:
                        mTouchX = (int) motionEvent.getX();
                        mTouchY = (int) motionEvent.getY();
                        int dx = mTouchX - view.getWidth() / 2;
                        int dy = mTouchY - view.getHeight() / 2;
                        try {
                            Game.instance().setDirection(dx, dy);
                        } catch (Exception ex) {
                            ex.printStackTrace();
                        }
                        return true;
                }
            return false;
        }

        class DrawThread extends Thread {

            GameDrawer drawer = new GameDrawer(getResources());
            private boolean running = false;
            private SurfaceHolder surfaceHolder;

            public DrawThread(SurfaceHolder surfaceHolder) {
                this.surfaceHolder = surfaceHolder;
            }

            public void setRunning(boolean running) {
                this.running = running;
            }

            @Override
            public void run() {
                final double MIN_FRAME_TIME = 10; // мс

                double lastFrameTime = 0;
                final float pixelSize = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_MM, 1,
                        getResources().getDisplayMetrics());

                while (running) {
                    double curFrameTime = System.currentTimeMillis() - lastFrameTime;
                    if (curFrameTime < MIN_FRAME_TIME) {
                        try {
                            sleep((int) (MIN_FRAME_TIME - curFrameTime));
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                    lastFrameTime = System.currentTimeMillis();

                    Canvas canvas = null;
                    try {
                        canvas = surfaceHolder.lockCanvas(null);
                        if (canvas == null)
                            continue;

                        Game.instance().run(canvas.getWidth() / pixelSize, canvas.getHeight() / pixelSize);

                        Field fld = Game.instance().getField();
                        drawer.draw(canvas, fld);

                        if(fld.getMySnake() == null && !gameOver) {
                            gameOver = true;
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    if(gameOver)
                                        mainMenu.setVisibility(View.VISIBLE);
                                }
                            });
                        }
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
