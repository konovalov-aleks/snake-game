import UIKit

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        initGestureRecognizers()
        StartGameLoop()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }
    
    func initGestureRecognizers() {
        let tapGR = UITapGestureRecognizer(target: self.view, action: #selector(onTouch))
        self.view.addGestureRecognizer(tapGR)
        let panGR = UIPanGestureRecognizer(target: self.view, action: #selector(onTouch))
        self.view.addGestureRecognizer(panGR)
    }
    
    func onTouch(gr: UIGestureRecognizer) {
        let touchPoint = gr.location(in: self.view)
        let dx: Int32 = Int32(touchPoint.x - self.view.frame.width / 2)
        let dy: Int32 = Int32(touchPoint.y - self.view.frame.height / 2)
        SDGame.instance()!.setDirection(dx, dy: dy)
    }
    
    func StartGameLoop() {

        SDGame.instance()!.enter()

        DispatchQueue.main.async {
            while true {
                SDGame.instance()!.run()
                let fld = SDGame.instance()!.getField()
                //let mySnakeHead = fld.mySnake.points[0]
                
            }
        }
    }
}

