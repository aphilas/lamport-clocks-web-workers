const NO_OF_WORKERS = 3

/**
 * Inline a web worker
 * @param {function} fn Function with web worker code to run
 * @returns {Worker}
 */
const createWorker = fn => {
  const src = `;(${fn})()`
  const blob = new Blob([src], {type: 'application/javascript'})
  const url = URL.createObjectURL(blob)
  return new Worker(url)
}

/**
 * Web worker code
 * 
 */
const runWorker = _ => {
  const NO_OF_WORKERS = 3
  
  const state = {
    sendPipes: [],
  }

  const nchoosek = (n,k) => { // SO
    let res = 1
    for(let i = 1; i <= k; i++){
      res *= (n + 1 - i) / i
    }
    return res
  }

  /**
   * Generate a random number between zero and n
   * @param {number} max Max number (inclusive)
   */
  const rand = max => Math.floor(Math.random() * (max + 1))

  const noOfCombinations = nchoosek(NO_OF_WORKERS, 2)

  /**
   * Send a message to a given worker
   * @param {*} message Message to send
   * @param {number} id Worker id
    */
  const send = (message, id) => {
    const pipe = state.sendPipes.find(pipe => pipe.id == id)

    if (pipe) {
      pipe.port.postMessage(message)
    } else {
      // console.log(`On worker ${state.id}`)
      console.error(`Pipe to ${id} not found`)
      // console.log(state.sendPipes)
    }
  }

  
  const run = _ => {
    // console.log(state.sendPipes)

    /**
     * Send message to all workers
     */
    for (const pipe of state.sendPipes) {
      send(`Hello worker ${pipe.id}, I am worker ${state.id}`, pipe.id)
    }

  }

  /**
   * receive postMessage() events
   */
  self.addEventListener('message', event => {
    const { sendPort, receivePort, workerPair, id } = event.data

    if (typeof id != 'undefined') {
      state.id = id // set id, first message sent to worker
      console.log(`Worker ${id} set up`)
    }

    if (typeof receivePort != 'undefined') {
      receivePort.start()

      // handle messages from other workers
      receivePort.addEventListener('message', event => {
        console.log(`Received: ${event.data}`)
      })
    }

    if (typeof sendPort != 'undefined') {
      /**
       * workerPair is array of two workers with a channel between them
       */
      state.sendPipes.push({
        id: workerPair.find(id => id != state.id),
        port: sendPort,
      })

      // console.log(`On worker ${state.id}, worker pair: `)
      // console.log(workerPair)
      // console.log(`Saved: ${workerPair.find(id => id != state.id)}`)
      // console.log(state)

      if (state.sendPipes.length == noOfCombinations - 1) { // FIX
        run()
      }
    }
  }, false)  
}

const workers = []

/**
 * Generate combinations of elements (not permutations)
 * @param {[*]} arr Array to generate combinations from
 * @returns {[[*]]} Array of combinations
 */
const combinations = arr => arr.flatMap((v, i) => arr.slice(i+1).map(w => [v, w]), 2) // SO

/**
 * Create n web workers, set worker id as index i
 */
for (let i = 0; i < NO_OF_WORKERS; i++) {
  // console.log(`Creating worker ${i}`)
  const worker = createWorker(runWorker)
  workers.push(worker)
  worker.postMessage({ id: i })
}

/**
 * Create all possible combinations of worker ids
 */
const workerCombinations = combinations(Array.from({ length: workers.length }, (v, i) => i))

/**
 * Set up bidirectional channel between worker combinations
 */
for (const [i1, i2] of workerCombinations) {
  const channel1 = new MessageChannel()
  const channel2 = new MessageChannel()

  workers[i1].postMessage({ receivePort: channel2.port1, sendPort: channel1.port2, workerPair: [i1, i2] }, [channel2.port1, channel1.port2])
  workers[i2].postMessage({ receivePort: channel1.port1, sendPort: channel2.port2, workerPair: [i1, i2] }, [channel1.port1, channel2.port2])
}