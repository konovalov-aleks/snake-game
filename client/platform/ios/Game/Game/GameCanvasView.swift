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
    let wallCGColor: CGColor = UIColor.yellow.cgColor
    
    let ppi = UIScreen.main.scale * ((UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiom.pad) ? 132 : 163)
    
    // Only override draw() if you perform custom drawing.
    // An empty implementation adversely affects performance during animation.
    override func draw(_ rect: CGRect) {
        var cameraPos = SDVectorModel(x: 0, y: 0)
        
        let ctx = UIGraphicsGetCurrentContext()!
        
        let gameInstance = SDGame.instance()!
        gameInstance.run()
        let fld = gameInstance.getField()
        let mySnake = fld.mySnake
        if mySnake != nil {
            cameraPos = mySnake!.points[0]
        }
        drawBackground(ctx: ctx, startPoint: cameraPos)
        
        ctx.translateBy(x: self.frame.width / 2 - mmToPoints(CGFloat(cameraPos.x)), y: self.frame.height / 2 - mmToPoints(CGFloat(cameraPos.y)))
        
        drawWalls(ctx: ctx, walls: fld.walls)
        
        // draw my snake
        if mySnake != nil {
            drawSnake(ctx: ctx, color: mySnakeCGColor, snake: mySnake!)
        }
        // draw other players' snake
        let snakes = fld.snakes
        for snake in snakes {
            drawSnake(ctx: ctx, color: otherSnakesCGColor, snake: snake)
        }
    }
    
    func drawWalls(ctx: CGContext, walls: SDWalls) {
        let scaledLeftX = mmToPoints(CGFloat(walls.leftX))
        let scaledRightX = mmToPoints(CGFloat(walls.rightX))
        let scaledTopY = mmToPoints(CGFloat(walls.topY))
        let scaledBottomY = mmToPoints(CGFloat(walls.bottomY))
        
        let wallsRectPath: UIBezierPath = UIBezierPath()
        wallsRectPath.move(to: CGPoint(x: scaledLeftX, y: scaledTopY))
        wallsRectPath.addLine(to: CGPoint(x: scaledRightX, y: scaledTopY))
        wallsRectPath.addLine(to: CGPoint(x: scaledRightX, y: scaledBottomY))
        wallsRectPath.addLine(to: CGPoint(x: scaledLeftX, y: scaledBottomY))
        wallsRectPath.close()
        
        ctx.saveGState()
        ctx.setLineWidth(CGFloat(mmToPoints(1.5)))
        ctx.setLineCap(.round)
        ctx.setLineJoin(.round)
        ctx.setStrokeColor(wallCGColor)
        ctx.setAlpha(0.5)
        ctx.addPath(wallsRectPath.cgPath)
        ctx.strokePath()
        ctx.restoreGState()
    }
    
    func drawBackground(ctx: CGContext, startPoint: SDVectorModel) {
        let w = Float(backgroundImage.width)
        let h = Float(backgroundImage.height)
        let x = -startPoint.x.truncatingRemainder(dividingBy: w) - w
        let y = -startPoint.y.truncatingRemainder(dividingBy: h) - h
        ctx.draw(backgroundImage, in: CGRect(x: mmToPoints(CGFloat(x)), y: mmToPoints(CGFloat(y)), width: CGFloat(w), height: CGFloat(h)), byTiling: true)
    }
    
    func drawEye(ctx: CGContext, position: SDVectorModel) {
        let whiteRegionSize = mmToPoints(0.7)
        let blackRegionSize = mmToPoints(0.3)
        
        ctx.saveGState()
        ctx.setFillColor(whiteCGColor)
        ctx.fillEllipse(in: CGRect(x: mmToPoints(CGFloat(position.x)) - whiteRegionSize / 2,
                                   y: mmToPoints(CGFloat(position.y)) - whiteRegionSize / 2, width: whiteRegionSize, height: whiteRegionSize))
        
        ctx.setFillColor(blackCGColor)
        ctx.fillEllipse(in: CGRect(x: mmToPoints(CGFloat(position.x)) - blackRegionSize / 2,
                                   y: mmToPoints(CGFloat(position.y)) - blackRegionSize / 2, width: blackRegionSize, height: blackRegionSize))
        ctx.restoreGState()
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
        bodyPath.move(to: CGPoint(x: mmToPoints(CGFloat(points[0].x)), y: mmToPoints(CGFloat(points[0].y))))
        for p in points {
            bodyPath.addLine(to: CGPoint(x: mmToPoints(CGFloat(p.x)), y: mmToPoints(CGFloat(p.y))))
        }
        ctx.saveGState()
        ctx.setStrokeColor(color)
        ctx.setLineJoin(.round)
        ctx.setLineCap(.round)
        ctx.setLineWidth(mmToPoints(4))
        ctx.setShouldAntialias(true)
        
        let cgBodyPath = bodyPath.cgPath
        ctx.addPath(cgBodyPath)
        ctx.strokePath()
        ctx.restoreGState()
        
        // draw eyes
        drawEye(ctx: ctx, position: snake.leftEye)
        drawEye(ctx: ctx, position: snake.rightEye)
    }
    
    func updateView() {
        setNeedsDisplay()
    }
    
    func mmToPoints(_ mm: CGFloat) -> CGFloat {
        // calculate pixels per mm
        let ppmm = ppi / 25.4
        let pixels = mm * ppmm
        return pixels / UIScreen.main.scale
    }
}
