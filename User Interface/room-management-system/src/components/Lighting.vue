<template>
  <div class="border-2 w-full h-full p-5">
    <div class="w-full mb-5">
      <div id="Blinds" class="">
        <div class="flex">
          <h3 class="w-1/3">East Window:</h3>
          <input type="range" min="0" max="180" v-model="blindsServo[0].position" class="flex-auto">
        </div>
        <div class="flex">
          <h3 class="w-1/3">South Window:</h3>
          <input type="range" min="0" max="180" v-model="blindsServo[1].position" class="flex-auto"> 
        </div>
        
      </div>
    </div>
    <div class="w-full">
      <div class="w-ful grid grid-cols-4 grid-rows-2 gap-2 my-2 text-black">
        <button v-on:click="setOff" class="rounded-md h-10 bg-gray-600">
          OFF
        </button>
        <button v-on:click="setWhite" class="rounded-md bg-white">
          White
        </button>
        <button v-on:click="setRed" class="rounded-md bg-red-600">
          RED
        </button>
        <button v-on:click="setGreen" class="rounded-md bg-green-600">
          GREEN
        </button>
        <button v-on:click="setBlue" class="rounded-md bg-blue-600">
          BLUE
        </button>
        <button v-on:click="setRainbow" class="rounded-md bg-pink-600">
          RAINBOW
        </button>
      </div>
      <div class="w-full">
        <div class="flex">
          <h3 class="w-1/3">Brightness:</h3>
          <input type="range" min="0" max="100" v-model="LEDstrip.brightness" class="flex-auto">
        </div>
        <div class="flex">
          <h3 class="w-1/3">Red:</h3>
          <input type="range" min="0" max="255" v-model="LEDstrip.red" class="flex-auto"> 
        </div>
        <div class="flex">
          <h3 class="w-1/3">Green:</h3>
          <input type="range" min="0" max="255" v-model="LEDstrip.green" class="flex-auto"> 
        </div>
        <div class="flex">
          <h3 class="w-1/3">Blue:</h3>
          <input type="range" min="0" max="255" v-model="LEDstrip.blue" class="flex-auto">  
        </div>
      </div>
    </div>
    
  </div>
</template>

<script>
export default {
  name: 'Lighting',
  props: {

  },
  data () {
    return {
      blindsServo: [
        {
          position: '180',
        },
        {
          position: '180',
        }
      ],
      LEDstrip: {
        brightness: '100',
        length: '142',
        green: '180',
        red: '80',
        blue: '200',
      }
    }
  },
  methods: {
    setRed: function () {
      this.LEDstrip.red = '255';
      this.LEDstrip.green = '0';
      this.LEDstrip.blue = '0';
    },
    setGreen: function () {
      this.LEDstrip.red = '0';
      this.LEDstrip.green = '255';
      this.LEDstrip.blue = '0';
    },
    setBlue: function () {
      this.LEDstrip.red = '0';
      this.LEDstrip.green = '0';
      this.LEDstrip.blue = '255';
    },
    setWhite: function () {
      this.LEDstrip.red = '255';
      this.LEDstrip.green = '255';
      this.LEDstrip.blue = '255';
    },
    setOff: function () {
      this.LEDstrip.red = '0';
      this.LEDstrip.green = '0';
      this.LEDstrip.blue = '0';
    },
    setRainbow: function() {

    }
  }
}
</script>

<style scoped>

</style>
