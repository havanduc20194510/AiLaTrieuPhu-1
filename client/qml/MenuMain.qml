import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Page {
  width: 480
  height: 640
  property bool showMenuMain: true
  property var sTatus : 0
  property bool clickShowPrizeBtn: false
  property var time : 15
  property var angle : 0
  

  SelectButtonPage{
    id : menuSelectButton
    _visible: showMenuMain
    btnMoveUp: sTatus == 1 ? true : false

    startY: 640
    textBtn1: "Chơi 1 mình"
    textBtn2: "Thi đấu online"
    textBtn3: "Đổi mật khẩu"
    textBtn4: "Đăng xuất"

    funcBtn1Click: btn1Click

    funcBtn3Click: btn3Click

    funcBtn4Click: btn4Click
  }

  Logo {
    visible: showMenuMain
    anchors.horizontalCenter: parent.horizontalCenter
  }

  Text {
    id: text
    visible: showMenuMain
    width: 220
    height: 50
    anchors.top: parent.top
    anchors.topMargin: 265
    anchors.horizontalCenter: parent.horizontalCenter
    font.bold: true
    font.pointSize: 16
    font.family: "roboto"
    text: "Hello, <font color='red'>" + backEnd.user_name + "</font>"
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter
    color: "white"
  }

  Text {
    id: text2
    visible: showMenuMain
    width: 220
    height: 50
    anchors.top: parent.top
    anchors.topMargin: 295
    anchors.horizontalCenter: parent.horizontalCenter
    font.bold: true
    font.pointSize: 16
    font.family: "roboto"
    text: "Score PvP:"
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter
    color: "white"
  }

  GameScreen {
    id : gameScreen
    btnMoveUp: sTatus == 2 ? true : false
  }

  PrizePopup{
    id: prizePopup
    modal : true

    onOpened : {
      if (!menuMain.clickShowPrizeBtn && menuMain.sTatus != 2){
        commericalBreakSound.stop()
        menuMain.resetBtnToStartY()
        prizeTheme.play()
        prizePopup.startHightLightPrize()
      }
      else if (!menuMain.clickShowPrizeBtn && menuMain.sTatus == 2){
        prizePopup.flickerPrize()
      }
      else {
        prize = backEnd.prize + 1
      }
    }
  }

  ResultPopup{
    id : resultPopup
  }

  function startBtnAnimUp(){
    gameScreen.startBtnAnimUp()
  }

  function startBtnAnimUpInMenuMain(){
    menuSelectButton.startBtnAnimUp()
  }

  function resetBtnToStartY(){
    gameScreen.resetBtnToStartY()
  }

  function flickerCorrectAnswer(count){
    gameScreen.flickerCorrectAnswer(count)
  }

  function showPrizePopup(){
    prizePopup.open()
  }

  function showResultPopup(){
    resultPopup.open()
  }

  function btn1Click(){
    rootWindow.lose = false
    menuSelectButton.startBtnAnimDown()
    backEnd.playAlone()
  }

  function btn3Click(){
    stackView.push("ChangePasswordPage.qml")
  }

  function btn4Click(){
    backEnd.logOut()
    stackView.pop()
  }
}