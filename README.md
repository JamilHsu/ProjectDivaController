此程式可以把iPad/安卓平板變成Hatsune Miku: Project Diva的控制器，類似Switch版的觸控遊玩功能。  
This application can transform your iPad/Android tablet into a controller for Hatsune Miku: Project Diva, providing a touch-based control experience similar to the Nintendo Switch version's Tap Play.

![image](https://raw.githubusercontent.com/JamilHsu/ProjectDivaController/refs/heads/master/ProjectDivaController%20Web%E9%81%8B%E4%BD%9C%E7%95%AB%E9%9D%A2.jpg)

使用方法:在電腦上啟動此程式，然後在平板上的瀏覽器網址列輸入"電腦IP":3939，例如`12.34.56.78:3939`。  
How to use: Launch this program on your computer, and then enter "your computer's IP address":3939 in the URL Bar of your browser on your iPad/tablet, for example, `12.34.56.78:3939`.

對於更多資訊。請參閱[ProjectDivaControllerServer][1]  
For more detailed information, please see [ProjectDivaControllerServer][1]

儘管這在Android上也能運作，不過安卓用戶建議選擇[ProjectDivaControllerServer][1] + [ProjectDivaControllerClient][2]的組合，本專案移植於此，並且在移植過程中不可避免的少了一點功能。  
Although this also works on Android, Android users are advised to choose the combination of [ProjectDivaControllerServer][1] + [ProjectDivaControllerClient][2]. This project is ported from this, and inevitably some features are missing during the porting process.

調整滑動專用區域的參數在WebClient/client.html裡的`const slider_height = 25;`  
The parameters for adjusting the dedicated slider area are located in WebClient/client.html: `const slider_height = 25;`

對於IOS用戶，可能會需要退出全螢幕模式。至於理由當你碰到後就知道了。  
For iOS users, you may need to exit full-screen mode. You'll find out why once you encounter it.

[1]: https://github.com/JamilHsu/ProjectDivaControllerServer
[2]: https://github.com/JamilHsu/ProjectDivaControllerClient
