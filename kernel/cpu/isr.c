#include "../include/common/helper.h"
#include "../include/cpu/isr.h"
#include "../include/cpu/util.h"
#include "../include/cpu/pic.h"
#include "../include/cpu/timer.h"
#include "../include/terminal/terminal.h"
#include "../include/drivers/keyboard.h"

// state_t interrupt_handlers[MAX_IDT_ENTRIES_COUNT];

// void isr_install() {
//     make_idt_entry(0, (uint32_t)isr0, 0x08, 0x8E);
//     make_idt_entry(1, (uint32_t)isr1, 0x08, 0x8E);
//     make_idt_entry(2, (uint32_t)isr2, 0x08, 0x8E);
//     make_idt_entry(3, (uint32_t)isr3, 0x08, 0x8E);
//     make_idt_entry(4, (uint32_t)isr4, 0x08, 0x8E);
//     make_idt_entry(5, (uint32_t)isr5, 0x08, 0x8E);
//     make_idt_entry(6, (uint32_t)isr6, 0x08, 0x8E);
//     make_idt_entry(7, (uint32_t)isr7, 0x08, 0x8E);
//     make_idt_entry(8, (uint32_t)isr8, 0x08, 0x8E);
//     make_idt_entry(9, (uint32_t)isr9, 0x08, 0x8E);
//     make_idt_entry(10, (uint32_t)isr10, 0x08, 0x8E);
//     make_idt_entry(11, (uint32_t)isr11, 0x08, 0x8E);
//     make_idt_entry(12, (uint32_t)isr12, 0x08, 0x8E);
//     make_idt_entry(13, (uint32_t)isr13, 0x08, 0x8E);
//     make_idt_entry(14, (uint32_t)isr14, 0x08, 0x8E);
//     make_idt_entry(15, (uint32_t)isr15, 0x08, 0x8E);
//     make_idt_entry(16, (uint32_t)isr16, 0x08, 0x8E);
//     make_idt_entry(17, (uint32_t)isr17, 0x08, 0x8E);
//     make_idt_entry(18, (uint32_t)isr18, 0x08, 0x8E);
//     make_idt_entry(19, (uint32_t)isr19, 0x08, 0x8E);
//     make_idt_entry(20, (uint32_t)isr20, 0x08, 0x8E);
//     make_idt_entry(21, (uint32_t)isr21, 0x08, 0x8E);
//     make_idt_entry(22, (uint32_t)isr22, 0x08, 0x8E);
//     make_idt_entry(23, (uint32_t)isr23, 0x08, 0x8E);
//     make_idt_entry(24, (uint32_t)isr24, 0x08, 0x8E);
//     make_idt_entry(25, (uint32_t)isr25, 0x08, 0x8E);
//     make_idt_entry(26, (uint32_t)isr26, 0x08, 0x8E);
//     make_idt_entry(27, (uint32_t)isr27, 0x08, 0x8E);
//     make_idt_entry(28, (uint32_t)isr28, 0x08, 0x8E);
//     make_idt_entry(29, (uint32_t)isr29, 0x08, 0x8E);
//     make_idt_entry(30, (uint32_t)isr30, 0x08, 0x8E);
//     make_idt_entry(31, (uint32_t)isr31, 0x08, 0x8E);

//     //     make_idt_entry(48, (uint32_t)isr48, 0x08, 0x8E);
//     // make_idt_entry(49, (uint32_t)isr49, 0x08, 0x8E);
//     // make_idt_entry(50, (uint32_t)isr50, 0x08, 0x8E);
//     // make_idt_entry(51, (uint32_t)isr51, 0x08, 0x8E);
//     // make_idt_entry(52, (uint32_t)isr52, 0x08, 0x8E);
//     // make_idt_entry(53, (uint32_t)isr53, 0x08, 0x8E);
//     // make_idt_entry(54, (uint32_t)isr54, 0x08, 0x8E);
//     // make_idt_entry(55, (uint32_t)isr55, 0x08, 0x8E);
//     // make_idt_entry(56, (uint32_t)isr56, 0x08, 0x8E);
//     // make_idt_entry(57, (uint32_t)isr57, 0x08, 0x8E);
//     // make_idt_entry(58, (uint32_t)isr58, 0x08, 0x8E);
//     // make_idt_entry(59, (uint32_t)isr59, 0x08, 0x8E);
//     // make_idt_entry(60, (uint32_t)isr60, 0x08, 0x8E);
//     // make_idt_entry(61, (uint32_t)isr61, 0x08, 0x8E);
//     // make_idt_entry(62, (uint32_t)isr62, 0x08, 0x8E);
//     // make_idt_entry(63, (uint32_t)isr63, 0x08, 0x8E);
//     // make_idt_entry(64, (uint32_t)isr64, 0x08, 0x8E);
//     // make_idt_entry(65, (uint32_t)isr65, 0x08, 0x8E);
//     // make_idt_entry(66, (uint32_t)isr66, 0x08, 0x8E);
//     // make_idt_entry(67, (uint32_t)isr67, 0x08, 0x8E);
//     // make_idt_entry(68, (uint32_t)isr68, 0x08, 0x8E);
//     // make_idt_entry(69, (uint32_t)isr69, 0x08, 0x8E);
//     // make_idt_entry(70, (uint32_t)isr70, 0x08, 0x8E);
//     // make_idt_entry(71, (uint32_t)isr71, 0x08, 0x8E);
//     // make_idt_entry(72, (uint32_t)isr72, 0x08, 0x8E);
//     // make_idt_entry(73, (uint32_t)isr73, 0x08, 0x8E);
//     // make_idt_entry(74, (uint32_t)isr74, 0x08, 0x8E);
//     // make_idt_entry(75, (uint32_t)isr75, 0x08, 0x8E);
//     // make_idt_entry(76, (uint32_t)isr76, 0x08, 0x8E);
//     // make_idt_entry(77, (uint32_t)isr77, 0x08, 0x8E);
//     // make_idt_entry(78, (uint32_t)isr78, 0x08, 0x8E);
//     // make_idt_entry(79, (uint32_t)isr79, 0x08, 0x8E);
//     // make_idt_entry(80, (uint32_t)isr80, 0x08, 0x8E);
//     // make_idt_entry(81, (uint32_t)isr81, 0x08, 0x8E);
//     // make_idt_entry(82, (uint32_t)isr82, 0x08, 0x8E);
//     // make_idt_entry(83, (uint32_t)isr83, 0x08, 0x8E);
//     // make_idt_entry(84, (uint32_t)isr84, 0x08, 0x8E);
//     // make_idt_entry(85, (uint32_t)isr85, 0x08, 0x8E);
//     // make_idt_entry(86, (uint32_t)isr86, 0x08, 0x8E);
//     // make_idt_entry(87, (uint32_t)isr87, 0x08, 0x8E);
//     // make_idt_entry(88, (uint32_t)isr88, 0x08, 0x8E);
//     // make_idt_entry(89, (uint32_t)isr89, 0x08, 0x8E);
//     // make_idt_entry(90, (uint32_t)isr90, 0x08, 0x8E);
//     // make_idt_entry(91, (uint32_t)isr91, 0x08, 0x8E);
//     // make_idt_entry(92, (uint32_t)isr92, 0x08, 0x8E);
//     // make_idt_entry(93, (uint32_t)isr93, 0x08, 0x8E);
//     // make_idt_entry(94, (uint32_t)isr94, 0x08, 0x8E);
//     // make_idt_entry(95, (uint32_t)isr95, 0x08, 0x8E);
//     // make_idt_entry(96, (uint32_t)isr96, 0x08, 0x8E);
//     // make_idt_entry(97, (uint32_t)isr97, 0x08, 0x8E);
//     // make_idt_entry(98, (uint32_t)isr98, 0x08, 0x8E);
//     // make_idt_entry(99, (uint32_t)isr99, 0x08, 0x8E);
//     // make_idt_entry(100, (uint32_t)isr100, 0x08, 0x8E);
//     // make_idt_entry(101, (uint32_t)isr101, 0x08, 0x8E);
//     // make_idt_entry(102, (uint32_t)isr102, 0x08, 0x8E);
//     // make_idt_entry(103, (uint32_t)isr103, 0x08, 0x8E);
//     // make_idt_entry(104, (uint32_t)isr104, 0x08, 0x8E);
//     // make_idt_entry(105, (uint32_t)isr105, 0x08, 0x8E);
//     // make_idt_entry(106, (uint32_t)isr106, 0x08, 0x8E);
//     // make_idt_entry(107, (uint32_t)isr107, 0x08, 0x8E);
//     // make_idt_entry(108, (uint32_t)isr108, 0x08, 0x8E);
//     // make_idt_entry(109, (uint32_t)isr109, 0x08, 0x8E);
//     // make_idt_entry(110, (uint32_t)isr110, 0x08, 0x8E);
//     // make_idt_entry(111, (uint32_t)isr111, 0x08, 0x8E);
//     // make_idt_entry(112, (uint32_t)isr112, 0x08, 0x8E);
//     // make_idt_entry(113, (uint32_t)isr113, 0x08, 0x8E);
//     // make_idt_entry(114, (uint32_t)isr114, 0x08, 0x8E);
//     // make_idt_entry(115, (uint32_t)isr115, 0x08, 0x8E);
//     // make_idt_entry(116, (uint32_t)isr116, 0x08, 0x8E);
//     // make_idt_entry(117, (uint32_t)isr117, 0x08, 0x8E);
//     // make_idt_entry(118, (uint32_t)isr118, 0x08, 0x8E);
//     // make_idt_entry(119, (uint32_t)isr119, 0x08, 0x8E);
//     // make_idt_entry(120, (uint32_t)isr120, 0x08, 0x8E);
//     // make_idt_entry(121, (uint32_t)isr121, 0x08, 0x8E);
//     // make_idt_entry(122, (uint32_t)isr122, 0x08, 0x8E);
//     // make_idt_entry(123, (uint32_t)isr123, 0x08, 0x8E);
//     // make_idt_entry(124, (uint32_t)isr124, 0x08, 0x8E);
//     // make_idt_entry(125, (uint32_t)isr125, 0x08, 0x8E);
//     // make_idt_entry(126, (uint32_t)isr126, 0x08, 0x8E);
//     // make_idt_entry(127, (uint32_t)isr127, 0x08, 0x8E);
//     // make_idt_entry(128, (uint32_t)isr128, 0x08, 0x8E);
//     // make_idt_entry(129, (uint32_t)isr129, 0x08, 0x8E);
//     // make_idt_entry(130, (uint32_t)isr130, 0x08, 0x8E);
//     // make_idt_entry(131, (uint32_t)isr131, 0x08, 0x8E);
//     // make_idt_entry(132, (uint32_t)isr132, 0x08, 0x8E);
//     // make_idt_entry(133, (uint32_t)isr133, 0x08, 0x8E);
//     // make_idt_entry(134, (uint32_t)isr134, 0x08, 0x8E);
//     // make_idt_entry(135, (uint32_t)isr135, 0x08, 0x8E);
//     // make_idt_entry(136, (uint32_t)isr136, 0x08, 0x8E);
//     // make_idt_entry(137, (uint32_t)isr137, 0x08, 0x8E);
//     // make_idt_entry(138, (uint32_t)isr138, 0x08, 0x8E);
//     // make_idt_entry(139, (uint32_t)isr139, 0x08, 0x8E);
//     // make_idt_entry(140, (uint32_t)isr140, 0x08, 0x8E);
//     // make_idt_entry(141, (uint32_t)isr141, 0x08, 0x8E);
//     // make_idt_entry(142, (uint32_t)isr142, 0x08, 0x8E);
//     // make_idt_entry(143, (uint32_t)isr143, 0x08, 0x8E);
//     // make_idt_entry(144, (uint32_t)isr144, 0x08, 0x8E);
//     // make_idt_entry(145, (uint32_t)isr145, 0x08, 0x8E);
//     // make_idt_entry(146, (uint32_t)isr146, 0x08, 0x8E);
//     // make_idt_entry(147, (uint32_t)isr147, 0x08, 0x8E);
//     // make_idt_entry(148, (uint32_t)isr148, 0x08, 0x8E);
//     // make_idt_entry(149, (uint32_t)isr149, 0x08, 0x8E);
//     // make_idt_entry(150, (uint32_t)isr150, 0x08, 0x8E);
//     // make_idt_entry(151, (uint32_t)isr151, 0x08, 0x8E);
//     // make_idt_entry(152, (uint32_t)isr152, 0x08, 0x8E);
//     // make_idt_entry(153, (uint32_t)isr153, 0x08, 0x8E);
//     // make_idt_entry(154, (uint32_t)isr154, 0x08, 0x8E);
//     // make_idt_entry(155, (uint32_t)isr155, 0x08, 0x8E);
//     // make_idt_entry(156, (uint32_t)isr156, 0x08, 0x8E);
//     // make_idt_entry(157, (uint32_t)isr157, 0x08, 0x8E);
//     // make_idt_entry(158, (uint32_t)isr158, 0x08, 0x8E);
//     // make_idt_entry(159, (uint32_t)isr159, 0x08, 0x8E);
//     // make_idt_entry(160, (uint32_t)isr160, 0x08, 0x8E);
//     // make_idt_entry(161, (uint32_t)isr161, 0x08, 0x8E);
//     // make_idt_entry(162, (uint32_t)isr162, 0x08, 0x8E);
//     // make_idt_entry(163, (uint32_t)isr163, 0x08, 0x8E);
//     // make_idt_entry(164, (uint32_t)isr164, 0x08, 0x8E);
//     // make_idt_entry(165, (uint32_t)isr165, 0x08, 0x8E);
//     // make_idt_entry(166, (uint32_t)isr166, 0x08, 0x8E);
//     // make_idt_entry(167, (uint32_t)isr167, 0x08, 0x8E);
//     // make_idt_entry(168, (uint32_t)isr168, 0x08, 0x8E);
//     // make_idt_entry(169, (uint32_t)isr169, 0x08, 0x8E);
//     // make_idt_entry(170, (uint32_t)isr170, 0x08, 0x8E);
//     // make_idt_entry(171, (uint32_t)isr171, 0x08, 0x8E);
//     // make_idt_entry(172, (uint32_t)isr172, 0x08, 0x8E);
//     // make_idt_entry(173, (uint32_t)isr173, 0x08, 0x8E);
//     // make_idt_entry(174, (uint32_t)isr174, 0x08, 0x8E);
//     // make_idt_entry(175, (uint32_t)isr175, 0x08, 0x8E);
//     // make_idt_entry(176, (uint32_t)isr176, 0x08, 0x8E);
//     // make_idt_entry(177, (uint32_t)isr177, 0x08, 0x8E);
//     // make_idt_entry(178, (uint32_t)isr178, 0x08, 0x8E);
//     // make_idt_entry(179, (uint32_t)isr179, 0x08, 0x8E);
//     // make_idt_entry(180, (uint32_t)isr180, 0x08, 0x8E);
//     // make_idt_entry(181, (uint32_t)isr181, 0x08, 0x8E);
//     // make_idt_entry(182, (uint32_t)isr182, 0x08, 0x8E);
//     // make_idt_entry(183, (uint32_t)isr183, 0x08, 0x8E);
//     // make_idt_entry(184, (uint32_t)isr184, 0x08, 0x8E);
//     // make_idt_entry(185, (uint32_t)isr185, 0x08, 0x8E);
//     // make_idt_entry(186, (uint32_t)isr186, 0x08, 0x8E);
//     // make_idt_entry(187, (uint32_t)isr187, 0x08, 0x8E);
//     // make_idt_entry(188, (uint32_t)isr188, 0x08, 0x8E);
//     // make_idt_entry(189, (uint32_t)isr189, 0x08, 0x8E);
//     // make_idt_entry(190, (uint32_t)isr190, 0x08, 0x8E);
//     // make_idt_entry(191, (uint32_t)isr191, 0x08, 0x8E);
//     // make_idt_entry(192, (uint32_t)isr192, 0x08, 0x8E);
//     // make_idt_entry(193, (uint32_t)isr193, 0x08, 0x8E);
//     // make_idt_entry(194, (uint32_t)isr194, 0x08, 0x8E);
//     // make_idt_entry(195, (uint32_t)isr195, 0x08, 0x8E);
//     // make_idt_entry(196, (uint32_t)isr196, 0x08, 0x8E);
//     // make_idt_entry(197, (uint32_t)isr197, 0x08, 0x8E);
//     // make_idt_entry(198, (uint32_t)isr198, 0x08, 0x8E);
//     // make_idt_entry(199, (uint32_t)isr199, 0x08, 0x8E);
//     // make_idt_entry(200, (uint32_t)isr200, 0x08, 0x8E);
//     // make_idt_entry(201, (uint32_t)isr201, 0x08, 0x8E);
//     // make_idt_entry(202, (uint32_t)isr202, 0x08, 0x8E);
//     // make_idt_entry(203, (uint32_t)isr203, 0x08, 0x8E);
//     // make_idt_entry(204, (uint32_t)isr204, 0x08, 0x8E);
//     // make_idt_entry(205, (uint32_t)isr205, 0x08, 0x8E);
//     // make_idt_entry(206, (uint32_t)isr206, 0x08, 0x8E);
//     // make_idt_entry(207, (uint32_t)isr207, 0x08, 0x8E);
//     // make_idt_entry(208, (uint32_t)isr208, 0x08, 0x8E);
//     // make_idt_entry(209, (uint32_t)isr209, 0x08, 0x8E);
//     // make_idt_entry(210, (uint32_t)isr210, 0x08, 0x8E);
//     // make_idt_entry(211, (uint32_t)isr211, 0x08, 0x8E);
//     // make_idt_entry(212, (uint32_t)isr212, 0x08, 0x8E);
//     // make_idt_entry(213, (uint32_t)isr213, 0x08, 0x8E);
//     // make_idt_entry(214, (uint32_t)isr214, 0x08, 0x8E);
//     // make_idt_entry(215, (uint32_t)isr215, 0x08, 0x8E);
//     // make_idt_entry(216, (uint32_t)isr216, 0x08, 0x8E);
//     // make_idt_entry(217, (uint32_t)isr217, 0x08, 0x8E);
//     // make_idt_entry(218, (uint32_t)isr218, 0x08, 0x8E);
//     // make_idt_entry(219, (uint32_t)isr219, 0x08, 0x8E);
//     // make_idt_entry(220, (uint32_t)isr220, 0x08, 0x8E);
//     // make_idt_entry(221, (uint32_t)isr221, 0x08, 0x8E);
//     // make_idt_entry(222, (uint32_t)isr222, 0x08, 0x8E);
//     // make_idt_entry(223, (uint32_t)isr223, 0x08, 0x8E);
//     // make_idt_entry(224, (uint32_t)isr224, 0x08, 0x8E);
//     // make_idt_entry(225, (uint32_t)isr225, 0x08, 0x8E);
//     // make_idt_entry(226, (uint32_t)isr226, 0x08, 0x8E);
//     // make_idt_entry(227, (uint32_t)isr227, 0x08, 0x8E);
//     // make_idt_entry(228, (uint32_t)isr228, 0x08, 0x8E);
//     // make_idt_entry(229, (uint32_t)isr229, 0x08, 0x8E);
//     // make_idt_entry(230, (uint32_t)isr230, 0x08, 0x8E);
//     // make_idt_entry(231, (uint32_t)isr231, 0x08, 0x8E);
//     // make_idt_entry(232, (uint32_t)isr232, 0x08, 0x8E);
//     // make_idt_entry(233, (uint32_t)isr233, 0x08, 0x8E);
//     // make_idt_entry(234, (uint32_t)isr234, 0x08, 0x8E);
//     // make_idt_entry(235, (uint32_t)isr235, 0x08, 0x8E);
//     // make_idt_entry(236, (uint32_t)isr236, 0x08, 0x8E);
//     // make_idt_entry(237, (uint32_t)isr237, 0x08, 0x8E);
//     // make_idt_entry(238, (uint32_t)isr238, 0x08, 0x8E);
//     // make_idt_entry(239, (uint32_t)isr239, 0x08, 0x8E);
//     // make_idt_entry(240, (uint32_t)isr240, 0x08, 0x8E);
//     // make_idt_entry(241, (uint32_t)isr241, 0x08, 0x8E);
//     // make_idt_entry(242, (uint32_t)isr242, 0x08, 0x8E);
//     // make_idt_entry(243, (uint32_t)isr243, 0x08, 0x8E);
//     // make_idt_entry(244, (uint32_t)isr244, 0x08, 0x8E);
//     // make_idt_entry(245, (uint32_t)isr245, 0x08, 0x8E);
//     // make_idt_entry(246, (uint32_t)isr246, 0x08, 0x8E);
//     // make_idt_entry(247, (uint32_t)isr247, 0x08, 0x8E);
//     // make_idt_entry(248, (uint32_t)isr248, 0x08, 0x8E);
//     // make_idt_entry(249, (uint32_t)isr249, 0x08, 0x8E);
//     // make_idt_entry(250, (uint32_t)isr250, 0x08, 0x8E);
//     // make_idt_entry(251, (uint32_t)isr251, 0x08, 0x8E);
//     // make_idt_entry(252, (uint32_t)isr252, 0x08, 0x8E);
//     // make_idt_entry(253, (uint32_t)isr253, 0x08, 0x8E);
//     // make_idt_entry(254, (uint32_t)isr254, 0x08, 0x8E);
//     // make_idt_entry(255, (uint32_t)isr255, 0x08, 0x8E);


//     // Remap the PIC
//     outb(0x20, 0x11);
//     outb(0xA0, 0x11);
//     outb(0x21, 0x20);
//     outb(0xA1, 0x28);
//     outb(0x21, 0x04);
//     outb(0xA1, 0x02);
//     outb(0x21, 0x01);
//     outb(0xA1, 0x01);
//     outb(0x21, 0x0);
//     outb(0xA1, 0x0); 

//     // Install the IRQs
//     make_idt_entry(32, (uint32_t)irq0, 0x08, 0x8E);
//     make_idt_entry(33, (uint32_t)irq1, 0x08, 0x8E);
//     make_idt_entry(34, (uint32_t)irq2, 0x08, 0x8E);
//     make_idt_entry(35, (uint32_t)irq3, 0x08, 0x8E);
//     make_idt_entry(36, (uint32_t)irq4, 0x08, 0x8E);
//     make_idt_entry(37, (uint32_t)irq5, 0x08, 0x8E);
//     make_idt_entry(38, (uint32_t)irq6, 0x08, 0x8E);
//     make_idt_entry(39, (uint32_t)irq7, 0x08, 0x8E);
//     make_idt_entry(40, (uint32_t)irq8, 0x08, 0x8E);
//     make_idt_entry(41, (uint32_t)irq9, 0x08, 0x8E);
//     make_idt_entry(42, (uint32_t)irq10, 0x08, 0x8E);
//     make_idt_entry(43, (uint32_t)irq11, 0x08, 0x8E);
//     make_idt_entry(44, (uint32_t)irq12, 0x08, 0x8E);
//     make_idt_entry(45, (uint32_t)irq13, 0x08, 0x8E);
//     make_idt_entry(46, (uint32_t)irq14, 0x08, 0x8E);
//     make_idt_entry(47, (uint32_t)irq15, 0x08, 0x8E);



//     init_idt();
// }

// /* To print the message which defines every exception */
// char *exception_messages[] = {
//     "Division By Zero",
//     "Debug",
//     "Non Maskable Interrupt",
//     "Breakpoint",
//     "Into Detected Overflow",
//     "Out of Bounds",
//     "Invalid Opcode",
//     "No Coprocessor",

//     "Double Fault",
//     "Coprocessor Segment Overrun",
//     "Bad TSS",
//     "Segment Not Present",
//     "Stack Fault",
//     "General Protection Fault",
//     "Page Fault",
//     "Unknown Interrupt",

//     "Coprocessor Fault",
//     "Alignment Check",
//     "Machine Check",
//     "Reserved",
//     "Reserved",
//     "Reserved",
//     "Reserved",
//     "Reserved",

//     "Reserved",
//     "Reserved",
//     "Reserved",
//     "Reserved",
//     "Reserved",
//     "Reserved",
//     "Reserved",
//     "Reserved"
// };

// void isr_handler(cpu_state *state) {
//     zprint("received interrupt: ");
//     char s[3];
//     int_to_ascii(state.int_no, s);
//     zprint(s);
//     zprint("\n");
//     zprint(exception_messages[state.int_no]);
//     zprint("\n");
// }

// void irq_handler(cpu_state *state) {
//     /* After every interrupt we need to send an EOI to the PICs
//      * or they will not send another interrupt again */
//     if (state.int_no >= 40) outb(0xA0, 0x20); /* slave */
//     outb(0x20, 0x20); /* master */

//     /* Handle the interrupt in a more modular way */
//     if (interrupt_handlers[state.int_no] != 0) {
//         state_t handler = interrupt_handlers[state.int_no];
//         handler(state);
//     }
// }

// void register_interrupt_handler(uint8_t n, state_t handler) {
//     interrupt_handlers[n] = handler;
// }

// void irq_install(){
// 	asm volatile("sti");
// 	init_timer(50);
//     init_keyboard();
// }

//            struct cpu_state {
//               uint32_t ds; /* Data segment selector */
//               uint32_t edi, esi, ebp, useless, ebx, edx, ecx, eax; /* Pushed by pusha. */
//               uint32_t int_no, err_code; /* Interrupt number and error code (if applicable) */
//               uint32_t eip, cs, eflags, esp, ss; /* Pushed by the processor automatically */
//            } __attribute__((packed));

#define INT_TIMER 0x00000008
#define INT_KEYBOARD 0x00000009
#define INT_GENERAL_PROTECTION_FAULT 0x0000000D
#define INT_PAGE_FAULT 0x0000000E

void interrupt_handler(struct cpu_state cpu) {
  print_log("\n!!! Interrupt\n");
  print_log("interrupt_number: ");
  print_log_int( cpu.int_no,10);
  print_log("\n");

  print_log("cpu.err_code: ");
  print_log_int( cpu.err_code,10);
  print_log("\n");

  // print_log("\nStack trace:\n");
  // print_log_int( eip);
  // print_log(" : ");
  // char * symbol_name = address_to_symbol_name(eip);
  // fprintf(LOG, symbol_name);
  // print_log("\n");
  // uint32_t ebp = cpu.ebp;
  // while (ebp & 0xC0100000) {
  //   uint32_t eip = ((uint32_t*) ebp)[1];
  //   print_log_int( eip);
  //   print_log(" : ");
  //   char * symbol_name = address_to_symbol_name(eip);
  //   fprintf(LOG, symbol_name);
  //   print_log("\n");

  //   ebp = *((uint32_t*)ebp);
  // }
  print_log("\n");

  switch(cpu.int_no) {

    case(INT_TIMER):{// keyboard_interrupt_handler();
        print_log("TIMER");
        timer_callback();
          break;
    }

    case(INT_KEYBOARD):{// keyboard_interrupt_handler();
        print_log("KEYBOARD");
        keyboard_callback();
          break;
    }
    case(INT_PAGE_FAULT):{  
          print_log("Interrupt was a page fault. Here's what I know:\n");
          print_log("- Tried to access linear address ");
          print_log_int( cpu.cr2,10);
          print_log("\n");
          if (cpu.err_code & 0x1) {
            print_log("- Couldn't complete because of page-protection violation\n");
          } else {
            print_log("- Couldn't complete because page was not present\n");
          }
          if (cpu.err_code & 0x2) {
            print_log("- This was an attempt to WRITE to this address.\n");
          } else {
            print_log("- This was an attempt to READ from this address.\n");
          }
          if (cpu.err_code & 0x4) {
            print_log("- Memory access came from user.\n");
          } else {
            print_log("- Memory access came from kernel.\n");
          }
          if (cpu.err_code & 0x08) {
            print_log("- caused by reading a 1 in a reserved field.\n");
          }
          if (cpu.err_code & 0x10) {
            print_log("- caused by an instruction fetch.\n");
          }

          // Cause Triple Fault !
          while(1){};
          break;
    }
    default:
      print_log("ERROR: Unabled to handle interrupt: ");
      print_log_int( cpu.int_no,10);
      print_log("\n");
      break;
  }

  pic_acknowledge();
}
