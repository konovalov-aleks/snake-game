import UIKit

class ViewController: UIViewController {
    
    // MARK: UIImageView outlet
    @IBOutlet weak var imageView: UIImageView!
    
    let backgroundImage: CGImage = UIImage(named: "Background")!.cgImage!
    

    override func viewDidLoad() {
        super.viewDidLoad()
        initGestureRecognizers()
        StartGameLoop()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }
    
    func initGestureRecognizers() {
        let tapGR = UITapGestureRecognizer(target: self, action: #selector(onTouch))
        self.view.addGestureRecognizer(tapGR)
        let panGR = UIPanGestureRecognizer(target: self, action: #selector(onTouch))
        self.view.addGestureRecognizer(panGR)
    }
    
    func onTouch(gr: UIGestureRecognizer) {
        let touchPoint = gr.location(in: self.imageView)
        let dx: Int32 = Int32(touchPoint.x - self.imageView.frame.width / 2)
        let dy: Int32 = Int32(touchPoint.y - self.imageView.frame.height / 2)
        SDGame.instance()!.setDirection(dx, dy: dy)
    }
    
    func drawBackground(ctx: CGContext, startPoint: SDVectorModel) {
        let w = Float(backgroundImage.width)
        let h = Float(backgroundImage.height)
        let x = -startPoint.x.truncatingRemainder(dividingBy: w) - w
        let y = -startPoint.y.truncatingRemainder(dividingBy: h) - h
        ctx.draw(backgroundImage, in: CGRect(x: CGFloat(x), y: CGFloat(y), width: CGFloat(w), height: CGFloat(h)), byTiling: true)
    }
    
    func drawSnake(ctx: CGContext, color: CGColor, snake: SDSnakeModel) {
        let points = snake.points
        if points.isEmpty {
            return
        }
        // draw snake body
        let bodyPath: UIBezierPath = UIBezierPath()
        bodyPath.lineJoinStyle = .round
        bodyPath.move(to: CGPoint(x: CGFloat(points[0].x), y: CGFloat(points[0].y)))
        for p in points {
            bodyPath.addLine(to: CGPoint(x: CGFloat(p.x), y: CGFloat(p.y)))
        }
        ctx.setStrokeColor(color)
        ctx.setLineJoin(.round)
        ctx.setLineCap(.round)
        ctx.setLineWidth(20)
        ctx.setShouldAntialias(true)
        
        bodyPath.stroke()
    }
    
    func StartGameLoop() {
        let mySnakeCGColor: CGColor = UIColor.blue.cgColor
        let otherSnakesCGColor: CGColor = UIColor.red.cgColor
        
        let gameInstance = SDGame.instance()!
        gameInstance.enter()
        
        DispatchQueue.global(qos: .background).async { [weak self] in
            while true {
                usleep(10000)
                gameInstance.run()
                let fld = gameInstance.getField()
                let center = fld.displayCenter
                
                // Render the picture on canvas (using UIImage CGContext)
                UIGraphicsBeginImageContextWithOptions((self?.imageView.frame.size)!, false, UIScreen.main.scale)
                let ctx = UIGraphicsGetCurrentContext()!

                self?.drawBackground(ctx: ctx, startPoint: center)
                
                // apply translation
                ctx.translateBy(x: (self?.imageView.frame.width)! / 2 - CGFloat(center.x), y: (self?.imageView.frame.height)! / 2 - CGFloat(center.y))
                
                // draw my snake
                let mySnake = fld.mySnake
                if mySnake != nil {
                    self?.drawSnake(ctx: ctx, color: mySnakeCGColor, snake: mySnake!)
                }
                // draw other players' snake
                let snakes = fld.snakes
                for snake in snakes {
                    self?.drawSnake(ctx: ctx, color: otherSnakesCGColor, snake: snake)
                }
                
                // extract UIImage from drawing context
                let renderedImg = UIGraphicsGetImageFromCurrentImageContext()!
                
                // assign it to main image view
                DispatchQueue.main.async {
                    [strongSelf = self] in
                    strongSelf!.imageView.image = renderedImg
                    strongSelf!.imageView.setNeedsDisplay()
                }
            }
        }
    }
}

