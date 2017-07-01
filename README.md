# XiaoFan Bot

这个 bot 完全不需要手动运行服务端环境，虽然目前酷 Q 是跑在阿里云上，不过通过魔法隧道、花生壳等内网穿透服务也可以在本地电脑上跑，然后事件直接上报到 IFTTT Maker Webhooks 服务，然后触发网络请求，POST 一个简短的启动代码到 compiler.run 在线运行，这段代码中 clone 本仓库，然后执行 `bash run.sh`。

流程：

```
酷 Q ----> HTTP API 插件 ----> IFTTT Maker Webhooks ----> compiler.run ----> clone 本仓库并运行 bash run.sh
```
