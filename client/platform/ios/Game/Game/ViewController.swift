import UIKit

class ViewController: UIViewController, GameCanvasViewDelegate {
    
    // MARK: Outlets
    
    @IBOutlet weak var startGameButton: UIButton!
    @IBOutlet weak var mainMenuView: UIView!
    
    // MARK: Константы
    
    let backgroundImage: CGImage = UIImage(named: "Background")!.cgImage!
    var updateTimer: Timer! = Timer()
    
    // MARK: Переменные
    
    var gameOver: Bool = true
    var tapGR: UITapGestureRecognizer!
    var panGR: UIPanGestureRecognizer!
    var longTapGR: UILongPressGestureRecognizer!
    
    // MARK: Действия
    
    @IBAction func onStartNewGame(_ sender: UIButton) {
        initGestureRecognizers()
        mainMenuView.isHidden = true
        startGameButton.isHidden = true
        startGame()
    }

    // MARK: Функции
    
    override func viewDidLoad() {
        super.viewDidLoad()
        (self.view as! GameCanvasView).delegate = self
        
        tapGR = UITapGestureRecognizer(target: self, action: #selector(onTouch))
        panGR = UIPanGestureRecognizer(target: self, action: #selector(onTouch))
        longTapGR = UILongPressGestureRecognizer(target: self, action: #selector(onTouch))
        longTapGR.minimumPressDuration = TimeInterval(0.1)
        
        scheduleCanvasUpdating()
    }
    
    override var prefersStatusBarHidden: Bool {
        return true
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }
    
    func initGestureRecognizers() {
        self.view.addGestureRecognizer(tapGR)
        self.view.addGestureRecognizer(panGR)
        self.view.addGestureRecognizer(longTapGR)
    }
    
    func clearGestureRecognizers() {
        self.view.removeGestureRecognizer(tapGR)
        self.view.removeGestureRecognizer(panGR)
        self.view.removeGestureRecognizer(longTapGR)
    }
    
    func onTouch(gr: UIGestureRecognizer) {
        DispatchQueue.global(qos: .userInitiated).async {
            let touchPoint = gr.location(in: self.view)
            let dx: Int32 = Int32(touchPoint.x - self.view.frame.width / 2)
            let dy: Int32 = Int32(touchPoint.y - self.view.frame.height / 2)
            SwiftTryCatch.tryRun({
                SDGame.instance()!.setDirection(dx, dy: dy)
            }, catchRun: { (error) in
                print("An error occurred while handling gesture recognition. Detailed error description: \(error)")
            }, finallyRun: nil)
        }
    }
    
    func startGame() {
        SwiftTryCatch.tryRun({
            SDGame.instance()!.enter()
            self.gameOver = false
            usleep(10000) // спим 10мс чтобы дать время создаться змейке
        }, catchRun: { (error) in
            print("An error occurred while entering the game. Detailed error description: \(error)")
        }, finallyRun: nil)
    }
    
    func scheduleCanvasUpdating() {
        updateTimer = Timer.scheduledTimer(timeInterval: 0.01, target: self.view, selector: #selector(GameCanvasView.updateView), userInfo: nil, repeats: true)
    }
    
    func handleGameOver() {
        let fld = SDGame.instance()!.getField()
        if fld.mySnake == nil && !gameOver {
            gameOver = true
            mainMenuView.isHidden = false
            startGameButton.isHidden = false
            clearGestureRecognizers()
        }
    }
}

