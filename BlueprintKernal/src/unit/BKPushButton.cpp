#include "unit/BKPushButton.h"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QFontMetrics>

class BKPushButton::Impl : public QGraphicsItem
{
public:
    Impl(BKPushButton* handle)
        : QGraphicsItem()
        , mpHandle(handle)
    {
        mOption.setWrapMode(QTextOption::NoWrap);
        mOption.setAlignment(Qt::AlignCenter);
    }

    ~Impl()
    {
    }

public:
    virtual QRectF boundingRect() const override
    {
        return mBoundingRect;
    }

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override
    {
        QRectF rect = mbPressed ? mButtonArea - QMargins(2, 2, 0, 0) : mButtonArea - QMargins(1, 1, 1, 1);

        // 绘制按钮
        painter->save();
        {
            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(255, 128, 64));
            painter->drawRoundedRect(rect, 1.0f, 1.0f);
            painter->restore();
        }

        // 绘制文字
        painter->save();
        {
            painter->setPen(Qt::white);
            painter->setBrush(Qt::NoBrush);
            painter->drawText(rect, mstrButtonText, mOption);
            painter->restore();
        }
    }

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override
    {
        if (event->button() == Qt::LeftButton)
        {
            if (mButtonArea.contains(event->pos().toPoint()))
            {
                mbPressed = true;
                update();
            }
        }
        event->accept();
    }

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override
    {
        if (event->button() == Qt::LeftButton)
        {
            if (mButtonArea.contains(event->pos().toPoint()))
            {
                // 触发
                // todo...
            }
            mbPressed = false;
            update();
        }
        event->accept();
    }

public:
    BKPushButton* mpHandle = nullptr;
    // 固定上下边距
    static constexpr int mFixedMargin = 2;
    // 包围盒
    QRectF mBoundingRect;
    // 按钮区域
    QRectF mButtonArea;
    // 是否为按下状态
    bool mbPressed = false;
    // 文字绘制选项
    QTextOption mOption;
    // 按钮文字
    QString mstrButtonText = "";
};

BKPushButton::BKPushButton()
    : super()
    , mpImpl(new Impl(this))
{
}

BKPushButton* BKPushButton::setText(const QString& text, bool comfy/* = false*/)
{
    L_IMPL(BKPushButton)

    l->mstrButtonText = text;

    if (comfy) {
        QFont defaultFont;
        QFontMetrics fm(defaultFont);           //assaber: 需要更新到系统字体
                                                //todo...
        setFixedSize({ 1.0f * fm.horizontalAdvance(text) + 4 * l->mFixedMargin, 1.0f * fm.height() + 4 * l->mFixedMargin });
    }

    return this;
}

QGraphicsItem* BKPushButton::getGraphicsItem()
{
    return mpImpl;
}

void BKPushButton::resized()
{
    L_IMPL(BKPushButton)

    l->mBoundingRect = { 0, 0, mSize.width(), mSize.height() };
    l->mButtonArea = { l->mFixedMargin, l->mFixedMargin, mSize.width() - 2 * l->mFixedMargin, mSize.height() - 2 * l->mFixedMargin };
}

void BKPushButton::dataChanged(const QVariant& data)
{
    // 格温不受影响
    /*

                                                               ?Cd]
                                                             )WL*C
                                                            vo+{$
                                                           QO^ {$
                                                          Ok:  {$
                                                          $x   [$,     .>vLCCYq&$@kbCY}
                                                          $x    dp^  ljoY{  (*w8Znxxxxxxxxr?'
                                                l||||||||1$x     tWxZL):   fx",)>>>>>>>>>>-jU0Y]I
                                             `}Ch1>>>>>>>iBY      ^>`                         ;1zqXi
                                           <Xbf;          u$        ^]                            +bX>
                                          (%<       ;}rxr:}%i        ;                             '1Zb]`
                                x\,      la?     _YZY|+>>'                                            !Uoj^
                               nk${    .Lo]   ;xbc!                                                      }b0<
                              I#<${    zZ^  "Xh{                                                           !zQQ|l
                             "bc ${    df  _kc"                                                               ^|Zk\`
                            .0*. ${   {*[uwC!                                                                    >tCUYUOY/||||||||)
                            }$"  ${   w@$mj                                                                             ^>>>>}L#Y?i
                            zZ   dv  n$J!                                                                                 ']wh\
                            qj   "@  $B`                                                                              '}tOQ|!
                     iwz^   $     bj_$n                                                                           :tz*qc_I
                     {$Xai  $     >$h|!                                                                   !YYYkdLYc{>^
                     {$'\%{ $      [h]                           :OUl                   i<                    Q@t
                     '$1 ,a/$       l#L,                        >afqm`                "L*~                    C$O,
                      UC  ;h$]       "Chi                      +$]  n8L?`           `rM$h)_i                   $8h+
                      (b   "m%n1^   ?OUwqw(                  ;np_    ^-fJCYLYj||||tmQ\` >||nCCYJUYYYYY}  >I    $x+oc`
                       pf    ^>{XLquo!   vQ                ivat`            .i>>>>>!          Yt."       Qx    $x `wx
                       zq`      I>B&>  ;n$\            '}uOQ-                                 Qxl&+      Qx   :$\  ro}
                       `$J_\OqbhLxL$qqqO$$0        ^/cM$c~                                l_> J0 Qc      0x   cpzJr~?8J"
                        {#MU1>!   nk    $Xh|    !YCa@1|o     ~Yj>.                    _fZLz\) i%X1M|     B:  'daOCYCwd%o|,
                       (au`      'h)    v%{$+     I$) xQ     `!_rLQY|~'           ;}CqX<^      l#zxQ    x$  ^qX      ^{kB}
                      -a!       Ipz      %t1ol   -WaL xQ           :_nahc        _$M(           ^boQ   +$Wf,pd.         [or
                      8/      l?w$-      >Wick. ]B$bz xQ         '?vphY)          ^_zqbr_        <$Q   X@QxYk            >$?
                      ${  "}QZU\Yddf-     cqlJ&>a@$$I xZ      !JZJj-`                 :?nYwCQr    cQ   $vQ*8t|||||||~^    um
                      $1+QqfI ;>>>_|xLove>,ox +*$$$$  ^pv                                        ,ph  t*!)r_>>>>>>>_tXOLt;.a1
                      cWa+  ]#C\|||tYqqJOvXLn  :)d$$   -$r               /$YYYYYYYY$]            w#W |k!               ^\bzOx
                      Yd'  \#,       "fma$o\'    m@$;  {$kO}'            {$       l${           mk$qWZi   '1xxxxr?`      `d$x
                     ]$"  _B-             [MWX` iW]$Y  {$'+jp\!          :%Y'   '}Wv'       '}naU?Y$}    tpu<>>>-faC       Wx
                     {$  lo?     !rYYYYYYu;lq$o]af $pb({$^  `[nCZ/-^      "cYYYYYf!    .l)c0Ctl    $     $'      co$       Qx
                     {$ _8}    .Q*{)>>1mLv&+'O@$b  ox>rm$[      ^~\ULLf             zJCJr?"        $     qv>>>>\#v^$       Qx
                     I%\x$     }$,      t8%x  |$f  xQ                "_?raqxxxQ$xxx1:              Z0.   ^fxxxxf, j$]      b|
                      ika$     {$        1$x'X#~   xQ                   lM!   {$`                  ^hmI     fYYYYJQr^     ja
                        z$     +$]        $bwf^   ;0Q               .[|JW)    'fkCYYYYYYYc<          tki                 -$>
                        1$?     +zkt|||tYqa+      C$t       "||cYYOCCj-:                 !U*\         -oUI             [fo_
                         $b       `>>>>>">'      jBv        mX>:                  nYYYr|I  lp0          _QCLc||||||tOUQz-`
                         8$w!                 l/Ww<       'nh      l>             w&> ~vWr  <$< ltxxr?'    '_______+`
                         ioYbm{"         `+/Yk$w~`}uYYYYYja0:  '|OQz$              LO   ^W/I L$$Bo$$*dkv
                          lt0*$$amYYYY0bo$$WZz~+Z%$$$$$%dwqd0||pv` I@               wY  `m$J>C$$$$$$$$$$Y
                                             I0@W$$BJYwBU   !@@f  Iqz               :wqZ%$$$$BW$$$$@#$$$$
                                             x$$$b<    $/  >oBm@J~*w                }&$$$$Z-^      f$$$$$
                                             8$$${     -zYYY{]%$$%Z               +Z$$$$$X         {$$$$$
                                             $$$$Y            "a$$$0+           ?q$$$$Z!lwU}'      ($$$$W
                                             t%$$$%bx|}l      io$$$$$Ui       1M$$$$d_    !tmO(>>1UB$$$C"
                                              ,uM$$B$$$$$#pwvz%ZJ$$$$$$m`    -8$$$&\      l]xL$o$$$$$B)
                                                 !\Lbb*$$$$$$@$$$$$$$$$$or||U$$$$*fYYYCh$$$$%$B$$$@0\"
                                                       .|*ar|cLkB$$@$$$$$$$$$$$$%$$B$$$$$$$$$$$#v]'
                                                     ~w0j->>>>>>>[L#88$$$$$$$$$$$$$$$$$$$$$$$8aLY{>`
                                                    iz$M@$$$$$$$$$$$$$$$$8$$$$$$M$$$$$$$$$$$$$$$a&Wopwr!
                                              .}YqMB$$$$$$$$$$$$$$$$$$$$$$$$$$*pMBB&$$$$$$$$$$$$$$$$$*oMoQjI
                                           "10@$$$$$$$$$$$$$$$$$$$$$$$$$$$$au+  l+/a@B$$$$$$$$$$$$$$$$$$$%pWdU_
                                          v$$$$$$$$$$$$$$$$$$$$$$$$$$$$$@u"         >L@$%$$$$$$$$$$$$$$$$$$$W**ZYI
                                        {M$$$$$$$$$$$$$$$$$$$$$$$$$$$$$m`             }h&B$$$$$$$$$$$$$$$$$$$$$#O#w:
                                       u$$$$$$$$$$$$$$$$$$$$$$$$$$$$$Wx                ^jM$$$$$$$$$$$$$$$$$$$$$$$W&p`
                                     ~d$$$$$$$$$$$$$$$$$$$$$$$$$$$$@Y'                   |pM&$$$$$$$$$$$$$$$$$$$$$Wba,
                                    ~@$$$$$$$$$$$$$$$$$$$$$$$$$$$$Zl                      .taWB$$$$$$$$$$$$$$$$$$$$%dMl.
                                   "W$$$$$$$$$$$$$$$$$$$$$$$$$$$w[                          ^8M$$$$$$$$$$$$$$$$$$$$$$#$@r
                                   _$$$$$$$$$$$$$$$$$$$$$$$$$$0~                             ,*a$$$$$$$$$$$$$$$$$$$$$$$$q
                                   n$$$$$$$$$$$$$$$$$$$$$$$$q_                                ;qa%$$$$$$$$$$$$$$$$$$$$$$q
                                  .$$$$$$$$$$$$$$$$$$$$$$$Bv.                                   ~kaB$$$$$$$$$$$$$$B$$$$$W`
                                  }$$$$$$$$$$$$$$$$$$$$$$M!                                      `fB$$$$$$$$$$$$$$W$$$$$$)
                                  u&$$$$$$$$$$$$$$$$$$$$%^                                         n$$$$$$$$$$$$$$$$$$$$$Q
                                  Q$$$$$$$$$$$$$$$$$$$$L                                            \*$$$$$$$$$$$$$$$$$$$&
                                  Q$$$$$$$$$$$$$$$$$@@Y`                                             .m$$$$$$$$$$$$$$$$$$&]
                                  Q$$$$$$$$$$$$$$$$$#<                                                .uW$$$$$$$$$$$$$$$$$x
                                  Q$$$$$$$$$$$$$$$%v^                                                   "k$$$$$$$$$$$$$$$$x
                                  Q$$$$$$$$$$$$$$z!                                                      !a$$$$$$$$$$$$$$$x
                                  Q$$$$$$$$$$$$X!                                                          1o$$$$#&$$$$$$$x
                                  vB@$$$$$$$$8|                                                              t%$$$$%$$$$$$x
                                  .Z$$$BB@$$q`                                                                <#$$$$$$$$$8<
                                   iB$$$$$%z'                                                                  .+\zk$$$$$B
                                   <M$$$8\,                                                                         ,_n*Bau,
                                   (W$Wj'                                                                                 i'
                                     O8
                                     `!
    */
    return;
}
