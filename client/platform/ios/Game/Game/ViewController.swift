import UIKit

class ViewController: UIViewController {
    
    let backgroundImage: CGImage = UIImage(named: "Background")!.cgImage!
    var updateTimer: Timer! = Timer()

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
        let touchPoint = gr.location(in: self.view)
        let dx: Int32 = Int32(touchPoint.x - self.view.frame.width / 2)
        let dy: Int32 = Int32(touchPoint.y - self.view.frame.height / 2)
        SDGame.instance()!.setDirection(dx, dy: dy)
    }
    
    func StartGameLoop() {
        let gameInstance = SDGame.instance()!
        gameInstance.enter()
        
        updateTimer = Timer.scheduledTimer(timeInterval: 0.01, target: self.view, selector: #selector(GameCanvasView.updateView), userInfo: nil, repeats: true)
    }
}

