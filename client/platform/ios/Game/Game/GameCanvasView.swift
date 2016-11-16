//
//  GameCanvasView.swift
//  Game
//
//  Created by Солодовников П.А. on 16.11.16.
//  Copyright © 2016 Tensor. All rights reserved.
//

import UIKit

class GameCanvasView: UIView {
    
    // MARK: Constants
    let backgroundImage: CGImage = UIImage(named: "Background")!.cgImage!
    let mySnakeCGColor: CGColor = UIColor.blue.cgColor
    let otherSnakesCGColor: CGColor = UIColor.red.cgColor
    let whiteCGColor: CGColor = UIColor.white.cgColor
    let blackCGColor: CGColor = UIColor.black.cgColor
    
    // Only override draw() if you perform custom drawing.
    // An empty implementation adversely affects performance during animation.
    override func draw(_ rect: CGRect) {
        let ctx = UIGraphicsGetCurrentContext()!
        
        let gameInstance = SDGame.instance()!
        gameInstance.run()
        let fld = gameInstance.getField()
        let center = fld.displayCenter
        
        drawBackground(ctx: ctx, startPoint: center)
        
        ctx.translateBy(x: self.frame.width / 2 - CGFloat(center.x), y: self.frame.height / 2 - CGFloat(center.y))
        
        // draw my snake
        let mySnake = fld.mySnake
        if mySnake != nil {
            drawSnake(ctx: ctx, color: mySnakeCGColor, snake: mySnake!)
        }
        // draw other players' snake
        let snakes = fld.snakes
        for snake in snakes {
            drawSnake(ctx: ctx, color: otherSnakesCGColor, snake: snake)
        }
    }
    
    func drawBackground(ctx: CGContext, startPoint: SDVectorModel) {
        let w = Float(backgroundImage.width)
        let h = Float(backgroundImage.height)
        let x = -startPoint.x.truncatingRemainder(dividingBy: w) - w
        let y = -startPoint.y.truncatingRemainder(dividingBy: h) - h
        ctx.draw(backgroundImage, in: CGRect(x: CGFloat(x), y: CGFloat(y), width: CGFloat(w), height: CGFloat(h)), byTiling: true)
    }
    
    func drawEye(ctx: CGContext, position: SDVectorModel) {
        ctx.setFillColor(whiteCGColor)
        ctx.fillEllipse(in: CGRect(x: CGFloat(position.x) - 2, y: CGFloat(position.y) - 2, width: CGFloat(4), height: CGFloat(4)))
        
        ctx.setFillColor(blackCGColor)
        ctx.fillEllipse(in: CGRect(x: CGFloat(position.x) - 1, y: CGFloat(position.y) - 1, width: CGFloat(2), height: CGFloat(2)))
    }
    
    func drawSnake(ctx: CGContext, color: CGColor, snake: SDSnakeModel) {
        let points = snake.points
        if points.isEmpty {
            return
        }
        // draw snake body
        let bodyPath: UIBezierPath = UIBezierPath()
        bodyPath.lineJoinStyle = .round
        bodyPath.lineCapStyle = .round
        bodyPath.move(to: CGPoint(x: CGFloat(points[0].x), y: CGFloat(points[0].y)))
        for p in points {
            bodyPath.addLine(to: CGPoint(x: CGFloat(p.x), y: CGFloat(p.y)))
        }
        ctx.setStrokeColor(color)
        ctx.setLineJoin(.round)
        ctx.setLineCap(.round)
        ctx.setLineWidth(20)
        ctx.setShouldAntialias(true)
        
        let cgBodyPath = bodyPath.cgPath
        ctx.addPath(cgBodyPath)
        ctx.strokePath()
        
        // draw eyes
        drawEye(ctx: ctx, position: snake.leftEye)
        drawEye(ctx: ctx, position: snake.rightEye)
    }
    
    func updateView() {
        setNeedsDisplay()
    }
}
