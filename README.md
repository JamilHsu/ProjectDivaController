此程式可以把iPad/安卓平板或任何具有觸控螢幕以及瀏覽器的裝置變成Hatsune Miku: Project Diva的控制器，類似Switch版的觸控遊玩功能。  
This application can transform your iPad/Android tablet or any device with a touchscreen and browser into a controller for Hatsune Miku: Project Diva, providing a touch-based control experience similar to the Nintendo Switch version's Tap Play.

![image](https://raw.githubusercontent.com/JamilHsu/ProjectDivaController/refs/heads/master/ProjectDivaController%20Web%E9%81%8B%E4%BD%9C%E7%95%AB%E9%9D%A2.jpg)

使用方法:在電腦上啟動此程式，然後在平板上的瀏覽器網址列輸入"電腦IP":3939，例如`192.168.50.122:3939`。
你可能會需要先將你的裝置與電腦置於同一個區域網路中，例如連接同一個wifi、裝置連接到電腦的行動熱點，或是使用USB線將電腦連上裝置的個人熱點(推薦)  
How to use: Launch this program on your computer, and then enter "your computer's IP address":3939 in the URL Bar of your browser on your device, for example, `192.168.50.122:3939` 
You may need to first put your device and computer on the same local area network, such as connecting to the same Wi-Fi, connecting your device to your computer's mobile hotspot, or using a USB cable to connect your computer to your device's personal hotspot (recommended).

對於更多資訊。請參閱[ProjectDivaControllerServer][1]  
For more detailed information, please see [ProjectDivaControllerServer][1]

儘管這在Android上也能運作，不過安卓用戶建議選擇[ProjectDivaControllerServer][1] + [ProjectDivaControllerClient][2]的組合，具有更好的效能。本專案移植於此，並且在移植過程中不可避免的少了一點功能。  
Although this also works on Android, Android users are advised to choose the combination of [ProjectDivaControllerServer][1] + [ProjectDivaControllerClient][2] ,which has slightly better performance. This project is ported from this, and inevitably some features are missing during the porting process.

調整滑動專用區域的參數在WebClient/client.html裡的`const slider_height = 25;`  
The parameters for adjusting the dedicated slider area are located in WebClient/client.html: `const slider_height = 25;`

對於IOS用戶，可能會需要退出全螢幕模式。至於理由當你碰到後就知道了。  
For iOS users, you may need to exit full-screen mode. You'll find out why once you encounter it. "It looks like you are typing while in full screen"

[1]: https://github.com/JamilHsu/ProjectDivaControllerServer
[2]: https://github.com/JamilHsu/ProjectDivaControllerClient
