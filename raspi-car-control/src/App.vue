<template>
  <main>
    <div class="flex flex-col items-center justify-center">
      
      <h1 class="text-2xl">Raspi Car Control</h1>
      <br>
      <iframe src="http://192.168.35.158:8081/0/stream" width="1280px" height="720px"></iframe>
    </div>
    <div class="flex items-center justify-center space-x-10">
      <div class="">
        <p>温度：{{ T }}</p>
        <p>湿度：{{ H }}</p>
        <p>气压：{{ P }}</p>
        <p>距离：{{ D }}</p>
      </div>
      <div class="flex">
        <el-text class=" w-14" >速度: {{ speed }}</el-text>
      </div>
      <div>
        <el-button @click="move('trace')">自动寻迹</el-button>
      </div>
    </div>
  </main>
</template>
<script>
import axios from 'axios';
export default {
  data() {
    return {  
      T:0,
      P:0,
      H:0,
      D:0,
      speed:150,
    }
  },
  mounted() {
    window.addEventListener('keydown', this.handleKeyDown);
    window.addEventListener('mousewheel',this.menu);
    clearInterval(this.intervalTime)
    this.intervalTime = setInterval(() => {
      this.sync_data();
      console.log(this.speed);
    }, 5000);
  },
  beforeUnmount() {
    window.removeEventListener('keydown', this.handleKeyDown);
  },
  methods: {
    sync_data(){
      axios.get("http://124.70.51.81:808/getBME").then(response =>{
        this.T=response.data.Temp;
        this.P=response.data.Pres;
        this.H=response.data.Hum;
        this.D=response.data.Dis;
      });
    },
    handleKeyDown(event) {
      console.log(event.key);
      if (event.key === 'W'||event.key === 'w') {
        this.move("ahead");
      }
      else if(event.key === 'A'||event.key === "a"){
        this.move("left");
      }
      else if(event.key === 'D'||event.key === 'd'){
        this.move("right");
      }
      else if(event.key === 'S'||event.key === 's'){
        this.move("stop");
      }
    },
    menu(e){
      if (e.wheelDelta) {  //判断浏览器IE，谷歌滑轮事件
        if (e.wheelDelta > 0) { //当滑轮向上滚动时
          this.speed++;
        }
        if (e.wheelDelta < 0) { //当滑轮向下滚动时
          this.speed--;
        }
      }
      else if (e.detail) {  //Firefox滑轮事件
        if (e.detail> 0) { //当滑轮向上滚动时
          this.speed++;
        }
        if (e.detail< 0) { //当滑轮向下滚动时
          this.speed--;
        }
      }
      if(this.speed>150){
        this.speed=150;
      }
      if(this.speed<0){
        this.speed=0;
      }
    },
    move(method){
      console.log(method);
      axios.get("http://124.70.51.81:808/move?method="+method+"&speed="+this.speed).then(response =>{
        console.log(response.data);
      });
    }
  }
}
</script>
<style scoped>
.el-input{
  width: 60px;
}
</style>