import { createApp } from 'vue'
import App from './App.vue'
import './index.css'
import {Axios} from 'axios'
import VueAxios from 'vue-axios'
import Element from 'element-plus'
import 'element-plus/dist/index.css'
const app = createApp(App)
app.use(Element)
app.use(VueAxios,Axios)
app.mount('#app')
