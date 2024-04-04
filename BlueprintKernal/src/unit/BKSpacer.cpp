#include "unit/BKSpacer.h"

class BKSpacer::Impl : public QGraphicsItem
{
public:
    Impl(BKSpacer* handle) : mpHandle(handle) {}

public:
    BKSpacer* mpHandle = nullptr;
   

public:
    QRectF boundingRect() const override
    {
        return { 0, 0, mpHandle->mSize.width(), mpHandle->mSize.height() };
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override
    {
        static_cast<void*>(widget);
        static_cast<void*>(painter);
    }
};

BKSpacer::BKSpacer()
    : super()
    , mpImpl(new Impl(this))
{
    setSizePolicy(SizePolicy::Adaptive);
}

BKSpacer::~BKSpacer()
{
    delete mpImpl;
    mpImpl = nullptr;
}

QJsonValue BKSpacer::getValue()
{
    return 0;
}

bool BKSpacer::setValue(const QJsonValue& val)
{
    return true;
}

QGraphicsItem* BKSpacer::getGraphicsItem()
{
    return mpImpl;
}

void BKSpacer::dataChanged(const QVariant& data)
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
