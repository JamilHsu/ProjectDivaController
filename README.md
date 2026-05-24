# Project Diva Controller

[中文](#中文) | [English](#english) | [Русский](#русский)

![](https://raw.githubusercontent.com/JamilHsu/ProjectDivaController/refs/heads/master/ProjectDivaController%20Web%E9%81%8B%E4%BD%9C%E7%95%AB%E9%9D%A2.jpg)

### 中文

此程式可以把iPad/安卓平板或任何具有觸控螢幕以及瀏覽器的裝置變成Hatsune Miku: Project Diva的控制器，類似Switch版的觸控遊玩功能。

使用方法:在電腦上啟動此程式，然後在平板上的瀏覽器網址列輸入"電腦IP":3939，例如`192.168.50.122:3939`。
你可能會需要先將你的裝置與電腦置於同一個區域網路中，例如連接同一個wifi、裝置連接到電腦的行動熱點，或是使用USB線將電腦連上裝置的個人熱點(推薦)

對於更多資訊。請參閱[ProjectDivaControllerServer][1]

儘管這在Android上也能運作，不過安卓用戶建議選擇[ProjectDivaControllerServer][1] + [ProjectDivaControllerClient][2]的組合，具有更好的效能。本專案移植於此，並且在移植過程中不可避免的少了一點功能。

調整滑動專用區域的參數在WebClient/client.html裡的`const slider_height = 25;`

對於IOS用戶，可能會需要退出全螢幕模式。至於理由當你碰到後就知道了。

已知在Safari/iOS 26上存在一些問題，導致WebSocket無法連線。使用者可能會需要嘗試換一個瀏覽器。詳情請自行上網搜尋相關內容。

---

### English

This application can transform your iPad/Android tablet or any device with a touchscreen and browser into a controller for Hatsune Miku: Project Diva, providing a touch-based control experience similar to the Nintendo Switch version's Tap Play.

How to use: Launch this program on your computer, and then enter "your computer's IP address":3939 in the URL bar of your browser on your device, for example, `192.168.50.122:3939`.
You may need to first put your device and computer on the same local area network, such as connecting to the same Wi-Fi, connecting your device to your computer's mobile hotspot, or using a USB cable to connect your computer to your device's personal hotspot (recommended).

For more detailed information, please see [ProjectDivaControllerServer][1]

Although this also works on Android, Android users are advised to choose the combination of [ProjectDivaControllerServer][1] + [ProjectDivaControllerClient][2], which has slightly better performance. This project is ported from this, and inevitably some features are missing during the porting process.

The parameters for adjusting the dedicated slider area are located in WebClient/client.html: `const slider_height = 25;`

For iOS users, you may need to exit full-screen mode. You'll find out why once you encounter it. "It looks like you are typing while in full screen"

It is known that there are some issues on Safari/iOS 26 that can prevent WebSocket connections from working. Users may need to try a different browser. Please search online for related details yourself.

---

### Русский

Это приложение позволяет превратить iPad/Android-планшет или любое устройство с сенсорным экраном и браузером в контроллер для Hatsune Miku: Project Diva, обеспечивая сенсорное управление, аналогичное режиму Tap Play в версии для Nintendo Switch.

Как использовать: запустите программу на компьютере, затем введите в адресную строку браузера на вашем устройстве "IP-адрес компьютера":3939, например `192.168.50.122:3939`.
Возможно, потребуется предварительно подключить устройство и компьютер к одной локальной сети — например, к одному Wi-Fi, подключить устройство к мобильной точке доступа компьютера или использовать USB-кабель для подключения компьютера к личной точке доступа устройства (рекомендуется).

Подробную информацию см. в [ProjectDivaControllerServer][1]

Хотя приложение работает и на Android, пользователям Android рекомендуется использовать связку [ProjectDivaControllerServer][1] + [ProjectDivaControllerClient][2] — она обеспечивает несколько лучшую производительность. Данный проект является портом этого решения, и в процессе портирования часть функций была неизбежно утрачена.

Параметры для настройки области свайпов находятся в WebClient/client.html: `const slider_height = 25;`

Пользователям iOS может потребоваться выйти из полноэкранного режима. Причина станет очевидна, когда вы с ней столкнётесь. «Похоже, вы печатаете в полноэкранном режиме»

В браузерах Safari/iOS 26 известны проблемы, препятствующие установлению WebSocket-соединений. Пользователям может потребоваться попробовать другой браузер. Для получения более подробной информации воспользуйтесь поиском в интернете.

[1]: https://github.com/JamilHsu/ProjectDivaControllerServer
[2]: https://github.com/JamilHsu/ProjectDivaControllerClient
