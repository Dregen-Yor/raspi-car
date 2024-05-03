<template>
  <main>
    <div class="flex flex-col items-center justify-center">
      <h1>Raspi Car Control</h1>
      <iframe src="http://127.0.0.1:8081/" width="400px" height="350px"></iframe>
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
    }
  },
  mounted() {
    clearInterval(this.intervalTime)
    this.intervalTime = setInterval(() => {
    }, 5000);
  },
  methods: {
    sync_data(){
      axios.get("http://127.0.0.1:5800/getT").then(response =>{
        this.T=response.data;
      });
      axios.get("http://127.0.0.1:5800/getP").then(response =>{
        this.P=response.data;
      });
      axios.get("http://127.0.0.1:5800/getH").then(response =>{
        this.H=response.data;
      });
      axios.get("http://127.0.0.1:5800/getD").then(response =>{
        this.D=response.data;
      });
    }
  }
}
</script>