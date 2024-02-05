clc; clear; close all;
warning off;
rng default;

cm = 1/100;
mm = 1/1000;

frame_size = 5*cm;
stemWidth = 1*mm;

[SO3, SE3] = lib_SO3_SE3_v3;

SPACE = eye(4);

%% Receive Socket
PORT = 27015;
u = udpport("IPV4","LocalHost","127.0.0.1",'LocalPort',PORT)

u2 = udp
u2.RemotePort = 27016
fopen(u2)

curr_q = zeros(1,7);
[robot, robotData] = loadrobot('kinovaGen3',"Version",2);
            URDFSource = robotData.Source;
            robot.Gravity = [0 0 -9.81];
            robot.DataFormat = 'column';
            q_0 = deg2rad([7.1637e-04 15.0060 179.9997 229.9965 360.0000 54.9953 89.9986]');

    az = 176.7295;
el =-1.5232;
robot_base = [0 0 0 0]

figure
timer1 = tic();
while toc(timer1) < 1000
    % Recv
    if(u.NumBytesAvailable>0)
        data = read(u,u.NumBytesAvailable,"string");
        cell1 = strsplit(data,'q_start');
        cell2 = strsplit(cell1{2},'q_end');
        q_cell = strsplit(cell2{1},'||');
        curr_q = [str2num(q_cell{1}) str2num(q_cell{2}) str2num(q_cell{3}) str2num(q_cell{4}) str2num(q_cell{5}) str2num(q_cell{6}) str2num(q_cell{7})];
        flush(u)
    end
    pos = curr_q(1:3)';
    quaternion = curr_q(4:end);
    R = quat2rotm(quaternion);
    pose = [R, pos; 0 0 0 1];

clf;
plot_frame_v2(frame_size,stemWidth,SPACE,SE3), hold on,
plot_frame_v2(frame_size,stemWidth,pose,SE3)
view([az el])
axis equal, grid on,drawnow
    %u.NumBytesAvailable

    % Send
    fwrite(u2,'soidfgnaijdbvjizdfbgkjsdfgndfjkni')
end

fclose(u2)
%% Send Socket

% fwrite(u,'aasfasf')