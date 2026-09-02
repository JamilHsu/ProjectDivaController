[English](#english)

![image](https://raw.githubusercontent.com/JamilHsu/ProjectDivaController/refs/heads/master/ProjectDivaController%20Web%E9%81%8B%E4%BD%9C%E7%95%AB%E9%9D%A2.jpg)

此程式可以把iPad/安卓平板或任何具有觸控螢幕以及瀏覽器的裝置變成Hatsune Miku: Project Diva的控制器，類似Switch版的觸控遊玩功能。  

使用方法:在電腦上啟動此程式，然後在裝置上的瀏覽器網址列輸入"電腦IP":3939，例如`192.168.137.1:3939`。
需要先將你的裝置與電腦置於同一個區域網路中，例如將電腦連上裝置的個人熱點(透過USB線尤佳)(iPad沒有行動數據不給開個人熱點，我未找到無須特殊硬體/越獄的有線方案)，將裝置連上電腦的行動熱點，或是電腦與裝置連上同一個基地台(延遲可能較高，可能高到玩不了音遊)  

儘管這在Android上也能運作，不過安卓用戶建議選擇[ProjectDivaControllerServer][1] + [ProjectDivaControllerClient][2]的組合，具有更好的效能及更多輔助功能。這個瀏覽器版純粹是因為iOS那該死的sideload限制而不得已弄的。

在ProjectDivaControllerSettings.txt中可以調整映射的鍵盤按鍵以及是否輸出WebSocket收到的訊息、按下的按鍵。

各項參數均硬編碼在client.html中，可以用記事本打開直接編輯。  
可自訂項目:  
黃色區域的高度比例(--slider-height-ratio)  
觸發滑動需要移動的距離(sliderRequire)，以CSS Pixel為單位。  
每毫秒減少的滑動距離(energyDecayRate)  
每當手指移動時，就會累積slideEnergy，如果達到sliderRequire，就會按下滑鍵。同時，每毫秒slideEnergy也會減少energyDecayRate的值，使緩慢的移動累積不起slideEnergy。  
簡而言之，較大的sliderRequire值表示要較長的移動距離才會觸發，較大的energyDecayRate值表示要較快的移動速度才能觸發。  
CSS Pixel有多大? 連線時主控台會輸出觸控區域的寬高，你可以拿尺自己在裝置上量一量，比對一下。  
sliderRequire**1**和energyDecayRate**1**用於上方黃色區域；而sliderRequire**2**和energyDecayRate**2**適用於下方按鍵區域。  

嚴格來說，其實除了與Server的協定之外，你什麼都能改，如果你有本事的話。畢竟HTML就擺在那裡。如果你真的很有本事的話，也可以給我發Pull Request哈。  
本專案有使用AI輔助生成。尤其是HTML的部分，是我將Android版的Kotlin餵給Claude，再以生出的結果為基底再修改的。

對於iOS用戶，在全螢幕下遊玩會一直出現「你似乎正在全螢幕模式中輸入」。解決辦法是加入主畫面以網頁App模式開啟。如果電腦IP變動了的話，必須重新從瀏覽器開啟並再次加入主畫面。 

iPad用戶建議開啟「引導使用模式」，並關閉「高效率手勢」。因不明原因，Safari有時會把觸控事件吞掉。你可以親自嘗試看看:用兩隻手指快速進行A-B-A 3下點擊，第2下經常會不見。開啟「引導使用模式」並關閉「高效率手勢」有時可以解決此問題。(這在我家的新iPad上可以解決此問題，但在舊iPad上無法)

隨附的dll和config.toml是一個微型輔助mod，此mod唯一的作用是自動啟動exe並使其在遊戲結束後終止。可以讓這個程式像mod一樣由mod管理器管理。此輔助mod也能使這個程式在Linux上無需特別設定即可運作。如果沒在使用mod的話，可以刪除dll和config.toml。

如有遇到任何問題，請在Discord  Project DIVA Modding 2nd  Multilingual-中文 裡@kb8522


### English

This program can turn an iPad, Android tablet, or any device with a touchscreen and a web browser into a controller for *Hatsune Miku: Project DIVA*, similar to the Tap Play feature in the Switch version.

Usage: Start this program on your computer, then enter `"computer IP":3939` in the URL bar of the browser on your device, for example, `192.168.137.1:3939`.

Your device and computer must first be on the same local area network. For example, you can connect the computer to the device's personal hotspot (USB connection is recommended) (iPads without cellular data cannot enable Personal Hotspot, and I have not found a wired solution that does not require special hardware or jailbreaking), connect the device to the computer's mobile hotspot, or connect both the computer and device to the same access point (latency may be higher and could potentially be too high for rhythm games).

Although this also works on Android, Android users are recommended to use the combination of [ProjectDivaControllerServer][1] + [ProjectDivaControllerClient][2], which offers better performance and more auxiliary features. This browser version was made purely because of iOS's damn sideloading restrictions, which left me with no other choice.

In `ProjectDivaControllerSettings.txt`, you can configure the keyboard keys used for mapping, as well as whether to output the messages received via WebSocket and the keys that are pressed.

All parameters are hard-coded in `client.html`, which you can open and edit directly with Notepad.  
Customizable options:  
The height ratio of the yellow area (`--slider-height-ratio`)  
The distance that needs to be moved to trigger a slide (`sliderRequire`), measured in CSS pixels.  
The amount of slide distance reduced per millisecond (`energyDecayRate`)

Whenever the finger moves, `slideEnergy` accumulates. When it reaches `sliderRequire`, a slide key is pressed. At the same time, `slideEnergy` is reduced by `energyDecayRate` every millisecond, preventing slow movements from accumulating enough `slideEnergy`.  
In short, a larger `sliderRequire` value means that a longer movement is required to trigger a slide, while a larger `energyDecayRate` value means that a faster movement is required to trigger it.  
How large is a CSS pixel? When connecting, the console will output the width and height of the touch area. You can measure it with a ruler on your device and compare the results.  
`sliderRequire1` and `energyDecayRate1` are used for the yellow area at the top; `sliderRequire2` and `energyDecayRate2` are used for the button area at the bottom.

Strictly speaking, aside from the protocol used to communicate with the Server, you can change pretty much anything if you know what you're doing. After all, the HTML is right there. If you're really skilled, feel free to send me a Pull Request.  
This project was created with the assistance of AI. In particular, for the HTML part, I fed the Kotlin code from the Android version to Claude and then modified the generated result as a base.

For iOS users, playing in fullscreen mode will repeatedly show the message “It seems that you are typing in fullscreen mode.” The solution is to add the page to the Home Screen and Open as Web App. If the computer's IP address changed, you must open the new IP in the browser again and add it to the Home Screen again.

iPad users are advised to enable “Guided Access” and disable “Productivity Gestures”. For some unknown reason, Safari sometimes swallows touch events. You can try it yourself: quickly perform three A-B-A taps using two fingers; the second tap will often disappear. Enabling “Guided Access” and disabling “Productivity Gestures” can sometimes solve this problem. (This fixes the issue on my newer iPad , but not on the older iPad.)

If you encounter any problems, please @kb8522 in the `diva-help` channel of Discord's Project DIVA Modding 2nd server.

[1]: https://github.com/JamilHsu/ProjectDivaControllerServer
[2]: https://github.com/JamilHsu/ProjectDivaControllerClient