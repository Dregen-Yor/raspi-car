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
        <el-text class=" w-14">速度: </el-text><el-input  v-model="speed" placeholder="速度范围为 0-100" maxlength="2"></el-input>
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
      speed:100,
    }
  },
  mounted() {
    window.addEventListener('keydown', this.handleKeyDown);
    clearInterval(this.intervalTime)
    this.intervalTime = setInterval(() => {
      this.sync_data();
      console.log("succ");
    }, 5000);
  },
  beforeUnmount() {
    window.removeEventListener('keydown', this.handleKeyDown);
  },
  methods: {
    sync_data(){
      axios.get("http://124.70.51.81:808/getT").then(response =>{
        this.T=response.data;
      });
      axios.get("http://124.70.51.81:808/getP").then(response =>{
        this.P=response.data;
      });
      axios.get("http://124.70.51.81:808/getH").then(response =>{
        this.H=response.data;
      });
      axios.get("http://124.70.51.81:808/getD").then(response =>{
        this.D=response.data;
      });
    },
    handleKeyDown(event) {
      if (event.key === 'W') {
        axios.post()
      }
    },
  }
}
</script>
<style scoped>
.el-input{
  width: 60px;
}
</style>