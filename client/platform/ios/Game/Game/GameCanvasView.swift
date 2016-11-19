//
//  GameCanvasView.swift
//  Game
//
//  Created by Солодовников П.А. on 16.11.16.
//  Copyright © 2016 Tensor. All rights reserved.
//

import UIKit

class GameCanvasView: UIView {
    
    // MARK: Константы
    let backgroundCGImage: CGImage = UIImage(named: "Background")!.cgImage!
    let mySnakeCGColor: CGColor = UIColor.blue.cgColor
    let otherSnakesCGColor: CGColor = UIColor.red.cgColor
    let whiteCGColor: CGColor = UIColor.white.cgColor
    let blackCGColor: CGColor = UIColor.black.cgColor
    let wallCGColor: CGColor = UIColor.yellow.cgColor
    
    static let ppi = UIScreen.main.scale * ((UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiom.pad) ? 132 : 163)
    static let screenDimsInMM = screenDimensionsInMM()
    
    public var delegate: GameCanvasViewDelegate?
    
    override func draw(_ rect: CGRect) {
        var cameraPos = SDVectorModel(x: 0, y: 0)
        
        let ctx = UIGraphicsGetCurrentContext()!
        ctx.setLineJoin(.round)
        ctx.setLineCap(.round)
        ctx.setAllowsAntialiasing(true)
        ctx.setShouldAntialias(true)
        
        let gameInstance = SDGame.instance()!
        gameInstance.run(Float(GameCanvasView.screenDimsInMM.width), dispHeight: Float(GameCanvasView.screenDimsInMM.height))
        let fld = gameInstance.getField()
        // настраиваем камеру
        let mySnake = fld.mySnake
        if mySnake != nil {
            cameraPos = mySnake!.points[0]
        }
        // отображаем задний фон
        //drawBackground(ctx: ctx, startPoint: cameraPos)
        
        ctx.translateBy(x: self.frame.width / 2 - mmToPoints(CGFloat(cameraPos.x)), y: self.frame.height / 2 - mmToPoints(CGFloat(cameraPos.y)))
        
        // рисуем стены
        drawWalls(ctx: ctx, walls: fld.walls)
        
        // отрисовка бонусов
        let bonuses = fld.bonuses
        for bonus in bonuses {
            drawBonus(ctx: ctx, bonus: bonus)
        }
        
        // Рисуем свою змейку
        if mySnake != nil {
            drawSnake(ctx: ctx, snake: mySnake!)
        }
        // Рисуем змейки других игроков
        let snakes = fld.snakes
        for snake in snakes {
            drawSnake(ctx: ctx, snake: snake)
        }
        // после отрисовки делегируем контроллеру определение того, закончилась игра или нет
        delegate?.handleGameOver()
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
        let w = Float(backgroundCGImage.width)
        let h = Float(backgroundCGImage.height)
        let x = -Float(mmToPoints(CGFloat(startPoint.x))).truncatingRemainder(dividingBy: w) - w
        let y = -Float(mmToPoints(CGFloat(startPoint.y))).truncatingRemainder(dividingBy: h) - h
        let targetRect = CGRect(x: CGFloat(x), y: CGFloat(y), width: CGFloat(w), height: CGFloat(h))
        ctx.draw(backgroundCGImage, in: targetRect, byTiling: true)
    }
    
    func drawEye(ctx: CGContext, position: SDVectorModel) {
        let whiteRegionSize = mmToPoints(1.4)
        let blackRegionSize = mmToPoints(0.6)
        
        ctx.saveGState()
        ctx.setFillColor(whiteCGColor)
        ctx.fillEllipse(in: CGRect(x: mmToPoints(CGFloat(position.x)) - whiteRegionSize / 2,
                                   y: mmToPoints(CGFloat(position.y)) - whiteRegionSize / 2, width: whiteRegionSize, height: whiteRegionSize))
        
        ctx.setFillColor(blackCGColor)
        ctx.fillEllipse(in: CGRect(x: mmToPoints(CGFloat(position.x)) - blackRegionSize / 2,
                                   y: mmToPoints(CGFloat(position.y)) - blackRegionSize / 2, width: blackRegionSize, height: blackRegionSize))
        ctx.restoreGState()
    }
    
    func drawBonus(ctx: CGContext, bonus: SDBonusModel) {
        var bonusRegionSize: CGFloat = mmToPoints(2)
        
        ctx.saveGState()
        for i in 1...3 {
            ctx.saveGState()
            // эмулируем LightingColorFilter из android
            var adjustedCol: UIColor = UIColor()
            
            adjustedCol = UIColor(red: CGFloat(bonus.color.red) / 255.0 * CGFloat(128 + i * 40) / 255,
                                          green: CGFloat(bonus.color.green) / 255.0 * CGFloat(128 + i * 30) / 255,
                                          blue: CGFloat(bonus.color.blue) / 255.0 * CGFloat(128 + i * 30) / 255, alpha: 0.6)
            bonusRegionSize = mmToPoints(2.0) - mmToPoints(CGFloat(0.2 * Double(i)))
            ctx.setFillColor(adjustedCol.cgColor)
            ctx.fillEllipse(in: CGRect(x: mmToPoints(CGFloat(bonus.position.x)) - bonusRegionSize / 2,
                                       y: mmToPoints(CGFloat(bonus.position.y)) - bonusRegionSize / 2, width: bonusRegionSize, height: bonusRegionSize))
            ctx.restoreGState()
        }
        ctx.restoreGState()
    }
    
    func drawSnake(ctx: CGContext, snake: SDSnakeModel) {
        let points = snake.points
        if points.isEmpty {
            return
        }
        // тело змеи
        let bodyPath: UIBezierPath = UIBezierPath()
        bodyPath.lineJoinStyle = .round
        bodyPath.lineCapStyle = .round
        bodyPath.move(to: CGPoint(x: mmToPoints(CGFloat(points[0].x)), y: mmToPoints(CGFloat(points[0].y))))
        for p in points {
            bodyPath.addLine(to: CGPoint(x: mmToPoints(CGFloat(p.x)), y: mmToPoints(CGFloat(p.y))))
        }
        ctx.saveGState()
        
        ctx.setShadow(offset: CGSize(width: 0, height: 0), blur: 2.5, color: blackCGColor)
        
        let cgBodyPath = bodyPath.cgPath
        
        for i in 1...3 {
            ctx.saveGState()
            ctx.setLineWidth(mmToPoints(CGFloat(3.0 - 0.4 * Double(i))))
            // эмулируем LightingColorFilter из android
            
            let adjustedCol = UIColor(red: CGFloat(snake.color.red) / 255.0 * CGFloat(128 + i * 40) / 255,
                                      green: CGFloat(snake.color.green) / 255.0 * CGFloat(128 + i * 30) / 255,
                                      blue: CGFloat(snake.color.blue) / 255.0 * CGFloat(128 + i * 30) / 255, alpha: 1.0)
            ctx.setStrokeColor(adjustedCol.cgColor)
            ctx.addPath(cgBodyPath)
            ctx.strokePath()
            ctx.restoreGState()
        }
        
        ctx.restoreGState()
        
        // глаза змеи
        drawEye(ctx: ctx, position: snake.leftEye)
        drawEye(ctx: ctx, position: snake.rightEye)
    }
    
    func updateView() {
        setNeedsDisplay()
    }
    
    // конвертация миллиметров в user points
    func mmToPoints(_ mm: CGFloat) -> CGFloat {
        let ppmm = GameCanvasView.ppi / 25.4
        let pixels = mm * ppmm
        return pixels / UIScreen.main.scale
    }
    
    static func screenDimensionsInMM() -> CGSize {
        let nativeBoundsRect = UIScreen.main.nativeBounds
        
        let ppmm = ppi / 25.4
        let mmWidth = nativeBoundsRect.maxX / ppmm
        let mmHeight = nativeBoundsRect.maxY / ppmm
        return CGSize(width: mmWidth, height: mmHeight)
    }
}
