module Main where 
import Control.Monad.Trans(liftIO)
import Graphics.UI.Gtk
import Graphics.Rendering.Cairo
import Data.IORef


-- ball
ball_y_init = 20 ::Double

win_width  = 640 :: Double
win_height = 400 :: Double
slime_radius = 40 :: Double
gravity    = 0.03

score_init = 50 :: Int

ball_radius =    10 :: Double
ball_serve_offset = 76.5
penalty_radius = 30 :: Double


data Ball = Ball {
  ball_x :: Double,
  ball_y :: Double,
  ball_vx :: Double,
  ball_vy :: Double
  } deriving(Show)
       
ball_new t =
  Ball {
    ball_x =  if t == Blue then
                ball_serve_offset
              else
                win_width - ball_serve_offset,
    ball_y =  ball_y_init,
    ball_vx = 0,
    ball_vy = 0
    }
      
ball_move b@Ball{ball_x=x, 
                 ball_y=y,
                 ball_vx=vx,
                 ball_vy=vy} = 
  reflection b{ball_x = x + vx,
               ball_y = y + vy,
               ball_vy = vy + gravity}
  where reflection b@Ball{ball_x = x, ball_vx = vx} 
          | x < 0 + ball_radius = b{ ball_x = (ball_radius - x) + ball_radius,
                                   ball_vx = -vx}
          | x > win_width - ball_radius =
            b{ball_x = (win_width - ball_radius) - 
                       (x - (win_width - ball_radius)),
              ball_vx = - vx}
          | otherwise = b
                                
data GameState = GS_INIT 
               | GS_PLAY 
               | GS_GOT_BY_BLUE 
               | GS_GOT_BY_RED  
               | GS_SERVICE_BY_BLUE
               | GS_SERVICE_BY_RED
               | GS_WON_BY_BLUE
               | GS_WON_BY_RED 
                 deriving(Show, Eq)

data GameInfo = GameInfo {
  gi_slime_blue :: Slime,
  gi_slime_red :: Slime,
  gi_ball :: Ball,
  --  window :: GtkWidget
  gi_state :: GameState,
  gi_score_blue :: Int,
  gi_score_red :: Int,
  gi_ball_count :: Int,
  gi_ball_owner :: Team,
  gi_penalty_y :: Double,
  gi_wait_count :: Int
  } deriving(Show)
            
gameinfo_new = GameInfo {
  gi_slime_blue = slime_new Blue,
  gi_slime_red  = slime_new Red,
  gi_ball = ball_new Blue,
  gi_state = GS_INIT,
  gi_score_blue = score_init,
  gi_score_red  = score_init,
  gi_ball_count = 0,
  gi_ball_owner = Blue,
  gi_penalty_y  = 0,
  gi_wait_count = 0
  }

data GameWin = GameWin {
  
  } deriving(Show)
           
data Team = Blue
          | Red
          deriving(Show, Eq)

data Slime = Slime {
  slime_team :: Team,
  slime_x :: Double,
  slime_vx :: Double,
  slime_limit_left ::  Double,
  slime_limit_right :: Double
  } deriving(Show)

slime_new team = slime_init Slime {
  slime_team = team,
  slime_x  = 0,
  slime_vx = 0,
  slime_limit_left = if team == Blue then
                       slime_radius
                     else
                       win_width / 2 + slime_radius,
  slime_limit_right = if team == Blue then
                        win_width / 2 - slime_radius
                      else
                        win_width - slime_radius
  }

slime_init Slime{
  slime_team=team,
  slime_x=x,
  slime_vx=vx,
  slime_limit_left=limit_left,
  slime_limit_right=limit_right
  } = Slime {
  slime_team=team,
  slime_x = if team == Blue then 
              slime_radius
            else 
              win_width - slime_radius,
  slime_vx=0,
  slime_limit_left=limit_left,
  slime_limit_right=limit_right
  }

slime_move s@Slime{slime_x=x,
                   slime_vx=vx,
                   slime_limit_left=limit_left,
                   slime_limit_right=limit_right} = 
  let x2 = x + vx in
  s {slime_x = case () of 
          _ | x2 < limit_left  -> limit_left
            | x2 > limit_right -> limit_right
            | otherwise  -> x2,
     slime_vx = if x2 < limit_left || x2 > limit_right then
                  0
                else
                  vx}
                   

pattern_blue = setSourceRGB 0 0 1
pattern_red = setSourceRGB 1 0 0 
pattern_ball = setSourceRGB 1 1 1
pattern_penalty = setSourceRGB 0 1 0

cb_expose_event iogameinfo = do
  drawWin <- eventWindow
  liftIO $ do
    gameinfo <- readIORef iogameinfo
    renderWithDrawable drawWin $ do
      setFontSize 16
      draw_back
      draw_slime $ gi_slime_blue gameinfo
      draw_slime $ gi_slime_red gameinfo
      draw_penalty $ gi_penalty_y gameinfo
      draw_ball $ gi_ball gameinfo
      draw_score (gi_score_blue gameinfo) (gi_ball_count gameinfo) 
        (gi_score_red gameinfo) 
      case (gi_state gameinfo) of
        GS_INIT -> return ()
        GS_PLAY -> return ()
        GS_GOT_BY_BLUE -> show_text_centering "DROPPED BY RED" 40
        GS_GOT_BY_RED -> show_text_centering "DROPPED BY BLUE" 40
        GS_SERVICE_BY_BLUE -> return ()
        GS_SERVICE_BY_RED  -> return ()
        GS_WON_BY_BLUE -> do
          show_text_centering "PRESS R TO PLAY AGAIN" 80
          show_text_centering "WON BY BLUE" 60
        GS_WON_BY_RED -> do
          show_text_centering "PRESS R TO PLAY AGAIN" 80
          show_text_centering "WON BY RED" 60
      return True
  where
    draw_back = do
      setSourceRGB 0 0 0
      rectangle 0 0 win_width win_height
      fill
      setSourceRGB 1 1 1
      rectangle (win_width/2 - 1) 120 2 win_height
      fill
    draw_slime s = do
      if (slime_team s) == Blue then
        do 
          pattern_blue
        else
        do
          pattern_red
      moveTo (slime_x s) win_height
      arc (slime_x s) win_height slime_radius (1 * pi) 0
      fill
    draw_ball b = do
      pattern_ball
      arc (ball_x b) (ball_y b) ball_radius 0 (2*pi)
      fill
    draw_penalty y = do
      setLineWidth 5.0
      pattern_penalty
      arc (win_width/2) y penalty_radius 0 (2*pi)
      strokePreserve
      
      setSourceRGB 0 0 0
      fill
      
      pattern_penalty
      arc (win_width/2) y (penalty_radius - 15) 0 (2*pi)
      strokePreserve
      
      setSourceRGB 0 0 0
      fill
    draw_score b c r = do
      show_text_centering ("Blue: " ++ (show b) ++ 
                           " Ball: " ++ (show c) ++ 
                           " Red: " ++ (show r)) 20
    show_text_centering t y = do
      te <- textExtents t
      moveTo ((win_width - textExtentsWidth te)/2) y
      showText t

cb_key_press_event window gameinfo = do 
  keyName <- eventKeyName
  [] <- eventModifier
  liftIO $ case keyName of 
    "r" -> modifyIORef gameinfo (restart)
    "a" -> modifyIORef gameinfo (update_gi Blue (-1))
    "s" -> modifyIORef gameinfo (update_gi Blue 1)
    "Left" ->  modifyIORef gameinfo (update_gi Red (-1))
    "Right" -> modifyIORef gameinfo (update_gi Red 1)
    "Escape" -> mainQuit
    _   -> return ()
  where
    restart gi@GameInfo{gi_state=state} 
      | (state ==  GS_WON_BY_RED || state == GS_WON_BY_BLUE) = gameinfo_new
      | otherwise = gi
    update_gi Blue vx gi@GameInfo{gi_slime_blue=sb@Slime{}} = 
      gi{gi_slime_blue=sb{slime_vx=vx}}
    update_gi Red vx gi@GameInfo{gi_slime_red=sr@Slime{}} = 
      gi{gi_slime_red=sr{slime_vx=vx}}

cb_timeout window gameinfo = do
  gi <- readIORef gameinfo
  case (gi_state gi) of
    GS_INIT -> modifyIORef gameinfo (update_state GS_SERVICE_BY_BLUE)
    GS_PLAY -> do
      modifyIORef gameinfo (update_slime Blue)
      modifyIORef gameinfo (update_slime Red)
      modifyIORef gameinfo (collision_ball_slime Blue)
      modifyIORef gameinfo (collision_ball_slime Red)
      modifyIORef gameinfo collision_penalty
      modifyIORef gameinfo update_ball
      modifyIORef gameinfo is_drop_ball
    GS_GOT_BY_BLUE -> modifyIORef gameinfo $ score_move Blue
    GS_GOT_BY_RED ->  modifyIORef gameinfo $ score_move Red
    GS_SERVICE_BY_BLUE -> modifyIORef gameinfo $ serve_set Blue
    GS_SERVICE_BY_RED  -> modifyIORef gameinfo $ serve_set Red
    GS_WON_BY_BLUE -> return ()
    GS_WON_BY_RED -> return ()
  widgetQueueDraw window
  return True
  where
    collision_ball_slime t gi@GameInfo{gi_ball=b@Ball{},
                                      gi_ball_count=count,
                                      gi_slime_blue=sb,
                                      gi_slime_red=sr} = 
      let s = if t == Blue then sb else sr
          v_s = (ball_x b, ball_y b)
          v_a = ((slime_x s) - (ball_x b), win_height - (ball_y b))
          v_b = ((slime_x s) - ((ball_x b) + (ball_vx b)), 
                 win_height - ((ball_y b)+(ball_vy b)))
          c = (inner_product v_a v_s) * (inner_product v_b v_s)
          d = (abs (outer_product v_s v_a) ) / norm(v_s)
          r = slime_radius + ball_radius in
      if d <= r && (c <= 0 || r > norm(v_a) || r > norm(v_b)) then
        let dx_0 = (ball_x b) - (slime_x s)
            dy_0 = (ball_y b) - win_height
            v = (dx_0, dy_0)
            dist_0 = norm(v)
            angle = acos(dx_0/dist_0)
            cos2x = cos(2*angle)
            sin2x = sin(2*angle) in
        gi{gi_ball=b{ball_vx =  (- ball_vx b) * cos2x + (ball_vy b) * sin2x,
                     ball_vy =   (ball_vx b) * sin2x + (ball_vy b) * cos2x},
           gi_ball_owner = t,
           gi_ball_count = count + 1}
      else
        gi
    collision_penalty gi@GameInfo{gi_ball=b@Ball{},
                                  gi_penalty_y = penalty_y,
                                  gi_score_blue = score_blue,
                                  gi_score_red = score_red,
                                  gi_ball_owner = owner,
                                  gi_wait_count = wait_count} = 
      let v = ((ball_x b) - win_width / 2, (ball_y b) - penalty_y) in
      if norm(v) < penalty_radius + ball_radius then
        reduce_score owner gi{ gi_wait_count= (wait_count -1)}
      else
        gi
      where
        reduce_score Blue gi@GameInfo{gi_score_blue=score,
                                      gi_wait_count=wait_count} = 
          gi{gi_score_blue=(score-1), gi_wait_count=4}
        reduce_score Red gi@GameInfo{gi_score_red=score,
                                      gi_wait_count=wait_count} = 
          gi{gi_score_red=(score-1), gi_wait_count=4}

    norm (a,b) = sqrt(a*a + b*b)
    inner_product (a,b) (c,d) = a * c + b * d
    outer_product (a,b) (c,d) = a * d - c * b
    update_state state gi@GameInfo{} = gi{gi_state=state}
    update_slime Blue gi@GameInfo{gi_slime_blue=sb@Slime{}} = 
      gi{gi_slime_blue=(slime_move sb)}
    update_slime Red gi@GameInfo{gi_slime_red=sr@Slime{}} = 
      gi{gi_slime_red=(slime_move sr)}

    update_ball gi@GameInfo{gi_ball=b@Ball{}} = 
      gi{gi_ball=(ball_move b)}
    is_drop_ball gi@GameInfo{gi_ball=b@Ball{ball_x=x, ball_y=y}} = 
      if y >= win_height - ball_radius then
        if x < win_width / 2 then
          gi{gi_state = GS_GOT_BY_RED}
        else
          gi{gi_state = GS_GOT_BY_BLUE}
      else
        gi
    serve_set team gi@GameInfo{gi_ball=ball,gi_slime_blue=sb,gi_slime_red=sr} = 
      update_state GS_PLAY gi{
        gi_ball = if team == Blue then
                    ball_new Blue
                  else
                    ball_new Red,
        gi_slime_blue=(slime_init sb),
        gi_slime_red=(slime_init sr),
        gi_penalty_y = 120, -- XXX
        gi_ball_count = 1,
        gi_wait_count = 0}
    score_move win gi@GameInfo{gi_ball_count=ball_count,
                           gi_wait_count=wait_count,
                           gi_score_blue=score_blue,
                           gi_score_red=score_red,
                           gi_state=state}
      | ball_count > 0 && wait_count > 0 = gi{gi_wait_count = (wait_count - 1)}
      | ball_count <= 0 && wait_count > 0 = gi{gi_wait_count = (wait_count - 1)}
      | ball_count <= 0 && wait_count <= 0 = 
        gi{gi_state = if win == Blue then 
                        GS_SERVICE_BY_BLUE 
                      else 
                        GS_SERVICE_BY_RED }
      | ball_count > 0 && wait_count <= 0 = 
        gi{gi_ball_count = (ball_count - 1),
           gi_wait_count = if ball_count == 0 then 50 else 7,
           gi_score_red = if win == Blue then score_red - 1 else score_red,
           gi_score_blue = if win == Red then score_blue - 1 else score_blue,
           gi_state = if win == Blue then
                         if score_red <= 0 then
                           GS_WON_BY_BLUE
                         else
                           state
                      else
                        if score_blue <= 0 then
                          GS_WON_BY_RED
                        else
                          state}
      
main = do
  initGUI
  gameinfo <- newIORef gameinfo_new
  window <- windowNew
  widgetSetSizeRequest window (round win_width) (round win_height)
  set window [ windowResizable := False]
  on window exposeEvent $ cb_expose_event gameinfo
  on window keyPressEvent $ tryEvent $ cb_key_press_event window gameinfo
  on window deleteEvent (liftIO $ (mainQuit >> return True))
  widgetShowAll window  
  timeoutAdd (cb_timeout window gameinfo) 10
  mainGUI

