import { spawn, spawnSync, execSync } from "child_process"

function remoteHasChanged() {

    const res = execSync("git fetch && git diff origin/master");
    console.log(res.toString());
    return res.toString().length > 0;
}

const dmxCmd = "./build/dmxSerial"

function rebuild() {
    let res = execSync("git pull --rebase");
    console.log("rebase", res.toString());
    res = execSync("bash build.sh");
    console.log("rebuilt", res.toString());
}


let child = undefined;
const respawn = () => {
    try {
    execSync("killall dmxSerial");
} catch { }
    child = spawn(dmxCmd, { shell: true })
    child.stdout.on('data', function (data) {
        console.log('stdout:' + data);
    });

    child.stderr.on('data', function (data) {
        console.log('stderr:' + data);
    });

}

respawn()

setInterval(
    () => {
        if (remoteHasChanged()) {
            console.log("should update");
            rebuild();
            console.log("--------------------------");
            respawn();
        }
    },1000
)


