%%
clf
clc, clear all
%===========================
%初始化gui
%设置按钮
mybutton=uicontrol('style','pushbutton',...
    'string','Stop', ...%stop
    'fontsize',12, ...%12
    'position',[200,400,50,20], ...
    'callback','freeze=1;');
my_button=uicontrol('style','pushbutton',...
    'string','Quit', ...%quit
    'fontsize',12, ...
    'position',[300,400,50,20], ...
    'callback','stop=1;close;');
my_button=uicontrol('style','pushbutton',...
    'string','Run', ...%run
    'fontsize',12, ...
    'position',[100,400,50,20], ...
    'callback', 'run=1;');
my_number = uicontrol('style','text', ...
    'string','1', ...%计数  重要
    'fontsize',12, ...
    'position',[20,400,50,20]);
%==============================
%%
%初始化
%网格
n=10;
m=50;
cheng_ke_shu=3000;
cheng_ke_dian=1;
z = ones(n,m);
imh = image(cat(3,z,z,z));
%标记数组
global flage
flage=zeros(2,50);
global tong_ji
%统计数据
tong_ji=zeros(2,1);
%第一个红色 代表汽车 第二个绿色 代表泊车位 第三个蓝色 代表乘客点

%泊车线 第1行第5行
z_bo=z;
data_bo=[38 35 32 29];
bo={};
jj=1;
for ii=1:size(data_bo,2)
    bo{jj}.x=data_bo(ii);
    bo{jj}.y=1;
    bo{jj}.flage=0;
    %是否有人去
    bo{jj}.ren=0;
    %上车时间
    bo{jj}.time1=0;
    %到车位时间
    bo{jj}.time2=0;
    jj=jj+1;
    bo{jj}.x=data_bo(ii);
    bo{jj}.y=5;
    bo{jj}.flage=0;%是否分配给汽车
    bo{jj}.ren=0;%是否分配给人
    bo{jj}.time1=0;%人上车时间
    bo{jj}.time2=0;%人走到车位的时间
    jj=jj+1;
end
z_bo=Ran_se(z_bo,bo);

%乘客点 第6行
%计算乘客上车时间
shi_jian=Shijian(cheng_ke_shu);
shi_jian=reshape(shi_jian,cheng_ke_dian,cheng_ke_shu/cheng_ke_dian);
%初始化
z_cheng=z;
data_bo=[35];%坐标
cheng={};
for ii=1:size(data_bo,2)
    cheng{ii}.x=data_bo(ii);
    cheng{ii}.y=6;
    cheng{ii}.list=shi_jian(ii,:);
end
z_cheng=Ran_se(z_cheng,cheng);

%初始化车
%两种状态ding 停车 zai 载人 wei 分配的车位
car={};
%奇数上
car{1}.x=1;
car{1}.y=2; 
%是否停稳了
car{1}.ting=0;
car{1}.zai=0;
car{1}.wei=1;
car{1}.yan_shi=2;
car{1}.jieshu=0;
%偶数下
car{2}.x=1;
car{2}.y=4;
car{2}.ting=0;
car{2}.zai=0;
car{2}.wei=2;
car{2}.yan_shi=2;
car{2}.jieshu=0;
z_car=Ran_se(z,car);

for ii=1:size(car,2)
    bo{car{ii}.wei}.flage=1;
end

imh = image(cat(3,z_car,z_bo,z_cheng));
set(imh, 'erasemode', 'none')
axis equal
axis tight
%%
stop= 0;
run = 0;

freeze = 0;
while (stop==0)
    if (run==1)
        %移动已经拉上课的出租车
        car=Move_jieshu_car(car);
        %移动未拉上客的出租车
        car=Move_car(car,bo);
        t_a=[];
        %绘制界面图像
        z_car=Ran_se(z,car);
        %生成新的车
        [car,bo]=New_car(car,bo,z_car);
        %为人分配车位
        [car,bo,cheng]=Shang_bo(car,bo,cheng);
        %模拟人走向车位
        bo=Ren_zou(bo);
        %模拟人上车
        car=Ren_che(car);
        %乘客上好后，车离开停车位，进入车道
        [car,bo]=Wu_yu(car,bo);
        
        if(tong_ji(1)==3000)
            stepnumber
            break;
        end
        %stepnumber当前时刻
        %存放在gui界面上
        stepnumber=str2num(get(my_number,'string'));
        if(rem(stepnumber,1000)==0)
            0000
            tong_ji
            0000
        end
%         if(stepnumber==6000)
%             stepnumber
%             tong_ji(1)=tong_ji(1)+150
%             break;
%         end
        %bo=Bu_ding(car,bo);
        set(imh, 'cdata', cat(3,z_car,z_bo,z_cheng) );
        stepnumber = 1 + str2num(get(my_number,'string'));
        set(my_number,'string',num2str(stepnumber))
    end
    if (freeze==1)
        run = 0;
        freeze = 0;
    end
    %     run=0;
    drawnow
end
%%
% function bos=Bu_ding(car,bo)
% cars=car;
% bos=bo;
% wei=[];
% for ii=1:size(cars,2)
%     wei=[wei,cars{ii}.wei];
% end
% for ii=1:size(bos,2)
%     if sum(wei==ii)~=1
%         bos{ii}.ren=0;
%         bos{ii}.flage=0;
%         bos{ii}.time1=0;
%         bos{ii}.time2=0;
%     end
% end
% end
%%
function [cars,bos]=Wu_yu(car,bo)
bos=bo;
cars=car;
global flage
global tong_ji
for ii=1:size(cars,2)
    if(cars{ii}.ting==0)
        continue
    end
    %车拉上客走了
    if(cars{ii}.yan_shi<=0)
        cars{ii}.ting=0;
        if car{ii}.y==4
            if flage(2,cars{ii}.x)==1
                tong_ji(2)=tong_ji(2)+1;
                continue
            end
        else
            if flage(1,cars{ii}.x)==1
                tong_ji(2)=tong_ji(2)+1;
                continue
            end
        end
        cars{ii}.jieshu=1;
        bos{cars{ii}.wei}.ren=0;
        bos{cars{ii}.wei}.flage=0;
        bos{cars{ii}.wei}.time1=0;
        bos{cars{ii}.wei}.time2=0;
    end
end
end
%%
function cars=Move_jieshu_car(car)
global flage
global tong_ji
cars=car;
temp=[];
for ii=1:size(cars,2)
    %判断是否载上车
    if cars{ii}.jieshu==1
        %移动
        if(cars{ii}.y==1)
            if(flage(1,cars{ii}.x)==1)
                tong_ji(2)=tong_ji(2)+1;
                continue
            end
            cars{ii}.y=2;
            flage(1,cars{ii}.x)=1;
            continue;
        end
        if(cars{ii}.y==5)
            if(flage(2,cars{ii}.x)==1)
                tong_ji(2)=tong_ji(2)+1;
                continue
            end
            cars{ii}.y=4;
            flage(2,cars{ii}.x)=1;
            continue;
        end
        %移动
        %判断是否会撞
        if car{ii}.y==4
            if flage(2,cars{ii}.x+1)==1
                tong_ji(2)=tong_ji(2)+1;
                continue
            end
        else
            if flage(1,cars{ii}.x+1)==1
                tong_ji(2)=tong_ji(2)+1;
                continue
            end
        end
        
        if car{ii}.y==4
            flage(2,cars{ii}.x)=0;
        else
            flage(1,cars{ii}.x)=0;
        end
        cars{ii}.x=cars{ii}.x+1;
        if car{ii}.y==4
            flage(2,cars{ii}.x)=1;
        else
            flage(1,cars{ii}.x)=1;
        end
        if(cars{ii}.x+1>48)
            %flage(cars{ii}.x)=0;
            temp=[temp ii];
        end
        continue
    end
end
for ii=temp(end:-1:1)
    if car{ii}.y==4
        flage(2,cars{ii}.x)=0;
    else
        flage(1,cars{ii}.x)=0;
    end
    tong_ji(1)=tong_ji(1)+1;
    cars(ii)=[];
end
end
%%
%人开始上车
function cars=Ren_che(car)
cars=car;
for ii=1:size(cars,2)
    if(cars{ii}.ting==1)
        cars{ii}.yan_shi=cars{ii}.yan_shi-1;
    end
    continue;
end
end
%%
%人开始走到车
function bos=Ren_zou(bo)
bos=bo;
for ii=1:size(bos,2)
    if(bos{ii}.ren==1)
        bos{ii}.time2=bos{ii}.time2-1;
    end
    continue;
end
end
%%
%上人
%人开始走到车位
function [cars,bos,chengs]=Shang_bo(car,bo,cheng)
cars=car;
bos=bo;
chengs=cheng;
%遍历排队口
for jj=1:size(chengs,2)
    p_x=chengs{jj}.x;
    p_y=chengs{jj}.y;
    p_t=chengs{jj}.list(end);
    bo_index=-9999;
    distance=99999;
    %找到最近的泊车点
    for kk=1:size(bos,2)
        %这个车位没人去
        if(bos{kk}.ren==1)
            continue
        end
        if(abs(bos{kk}.x-chengs{jj}.x)<distance)
            distance=abs(bos{kk}.x-chengs{jj}.x);
            bo_index=kk;
        end
    end
    %     chengs{jj}.list
    if bo_index~=-9999
        %这个人离开了
        chengs{jj}.list(end)=[];
        %给这个车位的时间
        bos{bo_index}.ren=1;
        %上车时间
        bos{bo_index}.time1=p_t;
        %走过去的时间
        bos{bo_index}.time2=abs(bos{bo_index}.x-p_x)/(93/60)+abs(rem(bo_index,2))*2/(93/60);
    end
end
end
%%
function [cars,bos]=New_car(car,bo,z_car)
n=0;
bos=bo;
if (z_car(4,1)==1)&&(z_car(4,2)==1)&&(z_car(4,3)==1)
    for ii=1:size(bo,2)
        if bo{ii}.flage==0
            bos{ii}.flage=1;
            temp=size(car,2)+1;
            car{temp}.x=1;
            
            if(bos{ii}.y==1)
                car{temp}.y=2;
            else
                car{temp}.y=4;
            end
            
            car{temp}.ting=0;
            car{temp}.zai=0;
            car{temp}.wei=ii;
            car{temp}.yan_shi=2;
            car{temp}.jieshu=0;
            n=n+1;
            if(n==2)
                break
            end
        end
    end
end
cars=car;
end
%%
function cars=Move_car(car,bo)
global flage
cars=car;
for ii=1:size(cars,2)
    if(cars{ii}.ting==0)
        if cars{ii}.x==bo{cars{ii}.wei}.x
            if cars{ii}.yan_shi==0
                if(cars{ii}.y==4)
                    flage(2,cars{ii}.x)=0;
                    cars{ii}.y=5;
                end
                if(cars{ii}.y==2)
                    flage(1,cars{ii}.x)=0;
                    cars{ii}.y=1;
                end
                
                %出租车停稳了
                %乘客上车了
                bo{cars{ii}.wei}.flage=1;
                cars{ii}.ting=1;
                if bo{cars{ii}.wei}.time2>=0
                    cars{ii}.yan_shi=3+round(bo{cars{ii}.wei}.time1+bo{cars{ii}.wei}.time2);
                else
                    cars{ii}.yan_shi=3+round(bo{cars{ii}.wei}.time1);
                end
            else
                %延时进车位
                cars{ii}.yan_shi=cars{ii}.yan_shi-1;
            end
            continue;
        end
        %直接移动
        if car{ii}.y==4
            if flage(2,cars{ii}.x+1)==1
                continue
            end
        else
            if flage(1,cars{ii}.x+1)==1
                continue
            end
        end
        
        if car{ii}.y==4
            flage(2,cars{ii}.x)=0;
        else
            flage(1,cars{ii}.x)=0;
        end
        cars{ii}.x=cars{ii}.x+1; %注意行列的变化
        if car{ii}.y==4
            flage(2,cars{ii}.x)=1;
        else
            flage(1,cars{ii}.x)=1;
        end
    end
end
end
%%
%染色函数
function cells=Ran_se(z,my_cell)
cells=z;
for ii=1:size(my_cell,2)
    cells(my_cell{ii}.y,my_cell{ii}.x)=0; %注意行列的变化
end
end
%%
%时间函数
function shi_jian=Shijian(N)
load shang_che.mat
x=shang_che(:,1);
v=shang_che(:,2);
xq=[0:0.00001:1];
vq = interp1(x,v,xq);
shi_jian=rand(N,1);
shi_jian=shi_jian-rem(shi_jian,0.00001);
shi_jian=vq(int32(shi_jian*100000));
end
